/*
** src/main.rs
*/

mod client;

use std::env;
use std::sync::{Arc, Condvar, Mutex};
use std::thread;

use lamp_protocol::{Owner, State};
use log::{debug, error};

type SharedState = Arc<(Mutex<State>, Condvar)>;

fn setup_logger() -> Result<(), fern::InitError> {
    fern::Dispatch::new()
        .format(|out, message, record| {
            out.finish(format_args!(
                "[{}][{}] {}",
                chrono::Local::now().format("%Y%m%dT%H:%M:%S"),
                record.level(),
                message
            ))
        })
        .level(log::LevelFilter::Debug)
        .chain(std::io::stdout())
        .chain(fern::log_file("output.log")?)
        .apply()?;

    Ok(())
}

fn owner_from_args() -> Result<Owner, String> {
    if let Some(owner) = env::args().nth(1) {
        match owner.as_str() {
            "arni" | "Arni" | "ARNI" => Ok(Owner::Arni),
            "ian" | "Ian" | "IAN" => Ok(Owner::Ian),
            _ => Err(format!("invalid owner \"{}\"", owner)),
        }
    } else {
        Err("missing argument OWNER".into())
    }
}

fn main() {
    // initialize logger
    if let Err(e) = setup_logger() {
        panic!("failed to initialize logger: {}", e);
    }

    // get the device owner from the command-line arguments and connect
    let owner = match owner_from_args() {
        Ok(owner) => owner,
        Err(reason) => {
            error!("error: {}", reason);
            return;
        }
    };

    // shared state between the client and device threads
    // written by the client thread, read by the device thread
    let device_state = Arc::new((Mutex::new(State::NotConnected), Condvar::new()));

    // spawn the client and device threads
    let client_handle = thread::spawn(move || {
        debug!("spawned client thread");
        client::run(device_state.clone(), owner);
        debug!("client thread terminating");
    });
    let device_handle = thread::spawn(move || {
        debug!("spawned device thread");
        debug!("device thread terminating");
    });

    let _ = client_handle.join();
    let _ = device_handle.join();
}
