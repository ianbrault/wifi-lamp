/*
** src/main.rs
*/

use std::convert::TryFrom;
use std::io::{self, Write};

use lamp_protocol::{ws_read, ws_write, ClientType, Command, Error, Owner, State};
use log::{info, error};
use tungstenite::connect;

type WebSocket = tungstenite::WebSocket<tungstenite::client::AutoStream>;

fn setup_logger() -> Result<(), log::SetLoggerError> {
    fern::Dispatch::new()
        .format(|out, message, _record| {
            out.finish(format_args!("{}", message))
        })
        .level(log::LevelFilter::Info)
        .chain(std::io::stdout())
        .apply()
}

fn connect_to_server() -> Result<WebSocket, Error> {
    let address = "ws://127.0.0.1:8182";
    info!("connecting to server at {}", address);

    let (mut websocket, _) = connect(address)?;
    info!("connected to server successfully");

    info!("sending DeclareClientType command for User (Ian)");
    let message = Command::declare_client_type(ClientType::User, Owner::Ian);
    ws_write(&mut websocket, message)?;

    info!("waiting for a DeclareClientTypeAck response");
    let message = ws_read(&mut websocket)?;
    let command = Command::try_from(message)?;
    if let Command::DeclareClientTypeAck = command {
        // success, as expected
        Ok(websocket)
    } else {
        Err(Error::unexpected_command("DeclareClientTypeAck", command.name()))
    }
}

fn try_parse_declare_client_type(rest: Vec<&str>) -> Result<Command, Error> {
    let usage = || {
        Error::invalid_command("DeclareClientType CLIENT_TYPE OWNER")
    };

    if let [cli_type_str, owner_str] = &rest[1..rest.len()] {
        let cli_type = match *cli_type_str {
            "Device" => Ok(ClientType::Device),
            "User"   => Ok(ClientType::User),
            _ => Err(usage()),
        }?;

        let owner = match *owner_str {
            "Arni" => Ok(Owner::Arni),
            "Ian"  => Ok(Owner::Ian),
            _ => Err(usage()),
        }?;

        Ok(Command::DeclareClientType(cli_type, owner))
    } else {
        Err(usage())
    }
}

fn try_parse_device_state_changed(rest: Vec<&str>) -> Result<Command, Error> {
    let usage = || {
        Error::invalid_command("DeviceStateChanged STATE")
    };

    if let [state_str] = &rest[1..rest.len()] {
        let state = match *state_str {
            "NotConnected"  => Ok(State::NotConnected),
            "ServerOffline" => Ok(State::ServerOffline),
            "Off"           => Ok(State::Off),
            "OnWaiting"     => Ok(State::OnWaiting),
            "OnPaired"      => Ok(State::OnPaired),
            _ => Err(usage()),
        }?;

        Ok(Command::DeviceStateChanged(state))
    } else {
        Err(usage())
    }
}

fn try_parse_command_string(input: String) -> Result<Command, Error> {
    // split on spaces and filter out empties
    let parts = input.as_str().trim()
        .split(' ')
        .filter(|s| !s.is_empty())
        .collect::<Vec<&str>>();

    if parts.is_empty() {
        return Err(Error::invalid_command("empty command"));
    }

    match parts[0] {
        "DeclareClientType"    => try_parse_declare_client_type(parts),
        "DeclareClientTypeAck" => Ok(Command::DeclareClientTypeAck),
        "DeviceStateChanged"   => try_parse_device_state_changed(parts),
        "PowerDeviceOff"       => Ok(Command::PowerDeviceOff),
        "PowerDeviceOn"        => Ok(Command::PowerDeviceOn),
        _ => Err(Error::invalid_command(
            format!("no command stem matching \"{}\"", parts[0])))
    }
}

fn run(mut websocket: WebSocket) -> Result<(), Error> {
    info!("\nenter commands as: STEM [ARGS ...]");
    loop {
        // print prompt
        // stdout must be flushed before input is read
        print!("cmd> ");
        io::stdout().flush()?;

        // read user input
        let mut input = String::new();
        let _ = io::stdin().read_line(&mut input)?;

        match try_parse_command_string(input) {
            Ok(command) => {
                // send the command
                if let Err(e) = ws_write(&mut websocket, command.into()) {
                    error!("error sending command: {}", e);
                }
            },
            Err(e) => {
                error!("error: {}", e);
            },
        }
    }
}

fn main() {
    // initialize logger
    if let Err(e) = setup_logger() {
        panic!("failed to initialize logger: {}", e);
    }

    // connect to the server and run interactively
    match connect_to_server() {
        Ok(websocket) => {
            if let Err(e) = run(websocket) {
                error!("client failure: {}", e);
            }
        },
        Err(e) => error!("failed to connect to the server: {}", e),
    }
}
