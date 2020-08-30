/*
** src/server.rs
*/

use std::io;
use std::net::{TcpListener, TcpStream};
use std::thread;

use lamp_protocol::Error;
use log::{debug, error, info};
use tungstenite::{accept, WebSocket};

fn handle_connection(mut websocket: WebSocket<TcpStream>) -> Result<(), Error> {
    // initial prototype: simple echo server
    loop {
        // read client message
        let message = websocket.read_message()?;
        debug!("received message: {:?}", message);

        // reply to binary messages
        if message.is_binary() {
            debug!("sending message: {:?}", message);
            websocket.write_message(message)?;
        } else {
            debug!("non-binary message received, ignoring");
        }
    }
}

// used to union the stream? and accept(...)? Err types when accepting a stream
fn accept_stream(stream: Result<TcpStream, io::Error>) -> Result<WebSocket<TcpStream>, Error> {
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
                Ok(websocket) => {
                    info!("stream accepted as WebSocket");
                    if let Err(err) = handle_connection(websocket) {
                        if err.closed_connection() {
                            info!("connection closed");
                        } else {
                            error!("error handling connection: {}", err);
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