/*
** src/client.rs
*/

use log::{debug, error, info};
use tungstenite::{connect, Error, Message, WebSocket};
use tungstenite::client::AutoStream;

fn handle_connection(mut websocket: WebSocket<AutoStream>) -> Result<(), Error> {
    // initial prototype: simple echo server
    loop {
        // send a binary message to the server
        let message = Message::Binary(vec![0xde, 0xad, 0xca, 0xfe]);
        debug!("sending message: {:?}", message);
        websocket.write_message(message)?;

        // read server response
        let message = websocket.read_message()?;
        debug!("received message: {:?}", message);
    }
}

pub fn run() {
    let address = "ws://127.0.0.1:8182";
    info!("connecting to server at {}", address);

    match connect(address) {
        Ok((websocket, _)) => {
            info!("connected to server successfully");
            if let Err(err) = handle_connection(websocket) {
                match err {
                    // connection closed normally
                    Error::ConnectionClosed => {
                        info!("connection closed");
                    },
                    // off-nominal error, log and terminate
                    _ => {
                        error!("error handling connection: {}", err);
                    }
                }
            }
        },
        Err(err) => {
            error!("failed to connect to server: {}", err);
        },
    }
}
