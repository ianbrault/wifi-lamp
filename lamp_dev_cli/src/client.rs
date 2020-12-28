/*
** src/client.rs
*/

use std::convert::TryFrom;

use lamp_protocol::{ws_read, ws_write, ClientType, Command, Error, Owner};
use log::{error, info};
use tungstenite::connect;

use crate::SharedState;

type WebSocket = tungstenite::WebSocket<tungstenite::client::AutoStream>;

fn handle_connection(
    mut websocket: WebSocket,
    shared_state: SharedState,
    owner: Owner,
) -> Result<(), Error> {
    info!("sending DeclareClientType command for Device ({})", owner);
    let message = Command::declare_client_type(ClientType::Device, owner);
    ws_write(&mut websocket, message)?;

    // wait for the DeclareClientTypeAck response
    info!("waiting for a DeclareClientTypeAck response");
    let message = ws_read(&mut websocket)?;

    let command = Command::try_from(message)?;
    if let Command::DeclareClientTypeAck = command {
        // success, as expected
    } else {
        return Err(Error::unexpected_command(
            "DeclareClientTypeAck",
            command.name(),
        ));
    }

    // loop and wait for DeviceStateChanged commands
    let (lock, cvar) = &*shared_state;
    loop {
        info!("waiting for a DeviceStateChanged command");
        let message = ws_read(&mut websocket)?;

        let command = Command::try_from(message)?;
        if let Command::DeviceStateChanged(new_state) = command {
            // update the local state and notify the GPIO thread
            let mut state = lock.lock().unwrap();
            *state = new_state;
            cvar.notify_all();
        } else {
            return Err(Error::unexpected_command(
                "DeviceStateChanged",
                command.name(),
            ));
        }
    }
}

pub fn run(shared_state: SharedState, owner: Owner) {
    let address = "ws://127.0.0.1:8182";
    info!("connecting to server at {}", address);

    match connect(address) {
        Ok((websocket, _)) => {
            info!("connected to server successfully");
            if let Err(err) = handle_connection(websocket, shared_state, owner) {
                if err.closed_connection() {
                    info!("connection closed");
                } else {
                    error!("error handling connection: {}", err);
                }
            }
        }
        Err(err) => {
            error!("failed to connect to server: {}", err);
        }
    }
}
