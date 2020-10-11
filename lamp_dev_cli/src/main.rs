/*
** src/main.rs
*/

mod client;

use std::env;

use lamp_protocol::Owner;
use log::error;

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
    if let Some(owner) = env::args().nth(0) {
        match owner.as_str() {
            "arni" | "Arni" | "ARNI" => Ok(Owner::Arni),
            "ian" | "Ian" | "IAN" => Ok(Owner::Ian),
            _ => Err(format!("invalid owner \"{}\"", owner))
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
    match owner_from_args() {
        Ok(owner) => client::run(owner),
        Err(reason) => error!("error: {}", reason),
    }
}
