/*
** src/client.rs
*/

use lamp_protocol::{message_as_hex, ClientType, Command, Error, Owner};
use log::{debug, error, info};
use tungstenite::connect;

type WebSocket = tungstenite::WebSocket<tungstenite::client::AutoStream>;

fn handle_connection(mut websocket: WebSocket) -> Result<(), Error> {
    // TODO: this should be fetched from the environment
    let owner = Owner::Ian;
    info!("sending DeclareClientType command for Device ({})", owner);
    let message = Command::declare_client_type(ClientType::Device, owner);
    debug!("sending message: {}", message_as_hex(&message));
    websocket.write_message(message)?;

    loop {
        // TODO
        // read should trigger closing the connection
        let _ = websocket.read_message()?;
    }
}

pub fn run() {
    let address = "ws://127.0.0.1:8182";
    info!("connecting to server at {}", address);

    match connect(address) {
        Ok((websocket, _)) => {
            info!("connected to server successfully");
            if let Err(err) = handle_connection(websocket) {
                if err.closed_connection() {
                    info!("connection closed");
                } else {
                    error!("error handling connection: {}", err);
                }
            }
        },
        Err(err) => {
            error!("failed to connect to server: {}", err);
        },
    }
}
