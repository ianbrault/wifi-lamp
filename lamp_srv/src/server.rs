/*
** src/server.rs
*/

use std::convert::TryFrom;
use std::io;
use std::net::{TcpListener, TcpStream};
use std::sync::{Arc, RwLock};
use std::thread;

use lamp_protocol::{ws_read, ws_write, ClientType, Command, Error, Notifier, Owner, State};
use log::{error, info};
use tungstenite::accept;

type SharedDeviceState = Arc<(RwLock<DeviceState>, Notifier)>;
type WebSocket = tungstenite::WebSocket<TcpStream>;

struct DeviceState {
    arni: State,
    ian: State,
}

impl DeviceState {
    fn new() -> Self {
        Self {
            arni: State::NotConnected,
            ian: State::NotConnected,
        }
    }

    fn get(&self, owner: Owner) -> &State {
        match owner {
            Owner::Arni => &self.arni,
            Owner::Ian => &self.ian,
        }
    }

    fn update(&mut self, owner: Owner, state: State) {
        let owner_state = match owner {
            Owner::Arni => &mut self.arni,
            Owner::Ian => &mut self.ian,
        };
        *owner_state = state;
    }

    fn power_device_on(&mut self, owner: Owner) {
        let partner = owner.partner();
        let other_state = self.get(partner);

        if other_state.is_on() {
            // both devices are now on and paired
            self.update(owner, State::OnPaired);
            self.update(partner, State::OnPaired);
        } else {
            // this device is on and waiting, no change to the partner
            self.update(owner, State::OnWaiting);
        }
    }

    fn power_device_off(&mut self, owner: Owner) {
        let partner = owner.partner();
        let other_state = self.get(partner);

        if other_state.is_on() {
            self.update(owner, State::Off);
            // the partner is now waiting
            self.update(partner, State::OnWaiting);
        } else {
            self.update(owner, State::Off);
        }
    }
}

fn update_shared_state(shared_state: SharedDeviceState, owner: Owner, state: State, notify: bool) {
    let (device_state, notifier) = &*shared_state;

    let mut state_guard = device_state.write().unwrap();
    state_guard.update(owner, state);

    if notify {
        notifier.notify_all();
    }
}

fn handle_device_connection(
    websocket: &mut WebSocket,
    state: SharedDeviceState,
    owner: Owner,
) -> Result<(), Error> {
    info!("Device ({}) connected", owner);

    // set device initial state: powered off
    // do not need to notify
    update_shared_state(state.clone(), owner, State::Off, false);

    let (device_state, notifier) = &*state;
    // enter event loop
    loop {
        // wait for an update to the device state by a user client
        notifier.wait();

        // read the new device state and push to the device
        let state_guard = device_state.read().unwrap();
        let new_state = state_guard.get(owner);

        info!("sending new state {:?} to Device ({})", new_state, owner);
        let command = Command::device_state_changed(new_state.clone());
        ws_write(websocket, command)?;
    }
}

fn handle_user_connection(
    websocket: &mut WebSocket,
    state: SharedDeviceState,
    owner: Owner,
) -> Result<(), Error> {
    info!("User ({}) connected", owner);
    let (device_state, notifier) = &*state;

    // enter event loop
    loop {
        // wait for a command from the user client
        let message = ws_read(websocket)?;

        let command = Command::try_from(message)?;
        // must be a power on/off command
        if command.is_device_command() {
            // update device state
            let mut state_guard = device_state.write().unwrap();
            match command {
                Command::PowerDeviceOn => state_guard.power_device_on(owner),
                Command::PowerDeviceOff => state_guard.power_device_off(owner),
                // unreachable due to command.is_device_command()
                _ => panic!("unreachable"),
            };

            // notify connected devices
            notifier.notify_all();
        } else {
            return Err(Error::unexpected_command(
                "PowerDeviceOn or PowerDeviceOff",
                command.name(),
            ));
        }
    }
}

fn handle_connection(websocket: &mut WebSocket, state: SharedDeviceState) -> Result<(), Error> {
    // wait for a DeclareClientType command
    info!("waiting for an initial DeclareClientType command");
    let message = ws_read(websocket)?;

    let command = Command::try_from(message)?;
    if let Command::DeclareClientType(client_type, owner) = command {
        // respond with a DeclareClientTypeAck command
        info!("sending back a DeclareClientTypeAck command");
        ws_write(websocket, Command::declare_client_type_ack())?;

        // pass off to individual client type handlers
        match client_type {
            ClientType::User => handle_user_connection(websocket, state, owner),
            // ensure that devices are reset to NotConnected when their connections are closed
            ClientType::Device => {
                let res = handle_device_connection(websocket, state.clone(), owner);
                update_shared_state(state, owner, State::NotConnected, true);
                res
            }
        }
    } else {
        Err(Error::unexpected_command(
            "DeclareClientType",
            command.name(),
        ))
    }
}

// used to union the stream? and accept(...)? Err types when accepting a stream
fn accept_stream(stream: Result<TcpStream, io::Error>) -> Result<WebSocket, Error> {
    let websocket = accept(stream?)?;
    Ok(websocket)
}

fn accept_connections(listener: TcpListener) {
    // device state is shared betweeen connection threads
    let device_state = Arc::new((RwLock::new(DeviceState::new()), Notifier::new()));

    // accept incoming connections as websockets
    for stream in listener.incoming() {
        let thread_state = device_state.clone();
        thread::spawn(move || {
            info!("incoming stream");
            match accept_stream(stream) {
                // connection accepted, dispatch to handler
                Ok(mut websocket) => {
                    info!("stream accepted as WebSocket");
                    if let Err(err) = handle_connection(&mut websocket, thread_state) {
                        if err.closed_connection() {
                            info!("connection closed");
                        } else {
                            error!("error handling connection: {}", err);
                            // close websocket connection
                            info!("closing connection to client");
                            // we don't really care about the Result, since it
                            // will fail if the connection is already closed
                            websocket.close(None).unwrap_or(());
                        }
                    }
                }
                // error accepting connection, log and terminate
                Err(err) => {
                    error!("failed to accept incoming connection: {}", err);
                }
            }
        });
    }
}

pub fn run() {
    let address = "127.0.0.1:8182";
    info!("binding TCP listener to {}", address);

    // initialize the TCP listener and accept client connections
    match TcpListener::bind(address) {
        Ok(listener) => accept_connections(listener),
        Err(e) => error!("failed to bind TCP listener: {}", e),
    }
}
