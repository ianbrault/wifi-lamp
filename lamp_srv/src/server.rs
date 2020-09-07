/*
** src/server.rs
*/

use std::convert::TryFrom;
use std::io;
use std::net::{TcpListener, TcpStream};
use std::sync::{Arc, Condvar, Mutex, RwLock};
use std::thread;

use lamp_protocol::{ws_read, ws_write, ClientType, Command, Error, Owner, State};
use log::{error, info};
use tungstenite::accept;

type SharedDeviceState = Arc<(RwLock<DeviceState>, Mutex<()>, Condvar)>;
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

    fn get(&self, owner: &Owner) -> &State {
        match owner {
            Owner::Arni => &self.arni,
            Owner::Ian => &self.ian,
        }
    }

    fn update(&mut self, owner: &Owner, state: State) {
        match owner {
            Owner::Arni => self.arni = state,
            Owner::Ian => self.ian = state,
        }
    }
}

fn handle_device_connection(websocket: &mut WebSocket, state: SharedDeviceState, owner: Owner)
    -> Result<(), Error>
{
    info!("Device ({}) connected", owner);
    let (device_state, guard, cvar) = &*state;

    // set device initial state: powered off
    // do not need to notify
    let mut state_guard = device_state.write().unwrap();
    state_guard.update(&owner, State::Off);
    drop(state_guard);

    // enter event loop
    loop {
        // wait for an update to the device state by a user client
        let cvar_guard = guard.lock().unwrap();
        let _cvar_guard = cvar.wait(cvar_guard).unwrap();

        // read the new device state and push to the device
        let state_guard = device_state.read().unwrap();
        let new_state = state_guard.get(&owner);

        info!("sending new state {:?} to Device ({})", new_state, owner);
        let command = Command::device_state_changed(new_state.clone());
        ws_write(websocket, command)?;
    }
}

fn handle_user_connection(_websocket: &mut WebSocket, state: SharedDeviceState, owner: Owner)
    -> Result<(), Error>
{
    info!("User ({}) connected", owner);
    let (_device_state, _guard, _cvar) = &*state;

    Err(Error::protocol_error("interface incomplete, terminating"))
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
            ClientType::Device => handle_device_connection(websocket, state, owner),
            ClientType::User   => handle_user_connection(websocket, state, owner),
        }
    } else {
        Err(Error::unexpected_command("DeclareClientType", command.name()))
    }
}

// used to union the stream? and accept(...)? Err types when accepting a stream
fn accept_stream(stream: Result<TcpStream, io::Error>) -> Result<WebSocket, Error> {
    let websocket = accept(stream?)?;
    Ok(websocket)
}

fn accept_connections(listener: TcpListener) {
    // device state is shared betweeen connection threads
    let device_state = Arc::new((
        RwLock::new(DeviceState::new()),
        Mutex::new(()),
        Condvar::new()));

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
                            websocket.close(None).unwrap();
                        }
                    }
                },
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
