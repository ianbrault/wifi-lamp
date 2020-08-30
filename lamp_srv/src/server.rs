/*
** src/server.rs
*/

use std::convert::TryFrom;
use std::io;
use std::net::{TcpListener, TcpStream};
use std::thread;

use lamp_protocol::{message_as_hex, ClientType, Command, Error, Owner};
use log::{debug, error, info};
use tungstenite::accept;

type WebSocket = tungstenite::WebSocket<TcpStream>;

fn handle_device_connection(websocket: &mut WebSocket, owner: Owner) -> Result<(), Error> {
    info!("client declared as Device ({})", owner);
    Err(Error::protocol_error("interface incomplete, terminating"))
}

fn handle_user_connection(websocket: &mut WebSocket, owner: Owner) -> Result<(), Error> {
    info!("client declared as User ({})", owner);
    Err(Error::protocol_error("interface incomplete, terminating"))
}

fn handle_connection(websocket: &mut WebSocket) -> Result<(), Error> {
    // wait for a DeclareClientType command
    info!("waiting for an initial DeclareClientType command");

    let message = websocket.read_message()?;
    debug!("received message: {}", message_as_hex(&message));

    let command = Command::try_from(message)?;
    if let Command::DeclareClientType(client_type, owner) = command {
        match client_type {
            ClientType::Device => handle_device_connection(websocket, owner),
            ClientType::User   => handle_user_connection(websocket, owner),
        }
    } else {
        Err(Error::Protocol(
            format!(
                "expected a DeclareClientType command but received a {} command instead",
                command.name())))
    }
}

// used to union the stream? and accept(...)? Err types when accepting a stream
fn accept_stream(stream: Result<TcpStream, io::Error>) -> Result<WebSocket, Error> {
    let websocket = accept(stream?)?;
    Ok(websocket)
}

fn accept_connections(listener: TcpListener) {
    // accept incoming connections as websockets
    for stream in listener.incoming() {
        thread::spawn(move || {
            info!("incoming stream");
            match accept_stream(stream) {
                // connection accepted, dispatch to handler
                Ok(mut websocket) => {
                    info!("stream accepted as WebSocket");
                    if let Err(err) = handle_connection(&mut websocket) {
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
