/*
** src/main.rs
*/

mod err;
mod server;

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

fn main() {
    // initialize logger
    if let Err(e) = setup_logger() {
        panic!("failed to initialize logger: {}", e);
    }

    server::run()
}
