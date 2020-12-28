/*
** src/lib.rs
*/

use std::convert::TryFrom;
use std::ffi::CStr;
use std::os::raw::c_char;
use std::ptr;

use lamp_protocol::{ws_read, ws_write, ClientType, Command, Error, Owner};

type WebSocket = tungstenite::WebSocket<tungstenite::client::AutoStream>;

unsafe fn ws_from_ptr(ptr: *mut WebSocket) -> &'static mut WebSocket {
    assert!(!ptr.is_null());
    &mut *ptr
}

// connect to the server and open a websocket
// returns a pointer to the opened websocket object
// WARNING: returns a null pointer if there was a connection error
#[no_mangle]
pub extern "C" fn connect(address: *const c_char) -> *mut WebSocket {
    // convert the provided address into a Rust-friendly string
    let address = unsafe {
        assert!(!address.is_null());
        match CStr::from_ptr(address).to_str() {
            Ok(s) => s,
            Err(_) => return ptr::null_mut(),
        }
    };

    // connect to the provided address
    match tungstenite::connect(address) {
        Ok((websocket, _)) => Box::into_raw(Box::new(websocket)),
        Err(_) => ptr::null_mut(),
    }
}

fn declare_client_connection_inner(ws: &mut WebSocket, owner: Owner) -> Result<(), Error> {
    // declare the client as a user
    let message = Command::declare_client_type(ClientType::User, owner);
    ws_write(ws, message)?;

    // wait for the DeclareClientTypeAck response
    let message = ws_read(ws)?;
    let command = Command::try_from(message)?;
    match command {
        Command::DeclareClientTypeAck => Ok(()),
        _ => Err(Error::unexpected_command(
            "DeclareClientTypeAck",
            command.name(),
        )),
    }
}

// declare the client type (user) and owner to the websocket server
// returns a boolean indicating whether or not the communication was successful
#[no_mangle]
pub extern "C" fn declare_client_connection(ws_ptr: *mut WebSocket, owner: u8) -> bool {
    let ws = unsafe { ws_from_ptr(ws_ptr) };

    let owner = match Owner::try_from(owner) {
        Ok(owner) => owner,
        Err(_) => return false,
    };

    declare_client_connection_inner(ws, owner).is_ok()
}

// turn the lamp on
#[no_mangle]
pub extern "C" fn power_on(ws_ptr: *mut WebSocket) -> bool {
    let ws = unsafe { ws_from_ptr(ws_ptr) };
    ws_write(ws, Command::PowerDeviceOn.into()).is_ok()
}

// turn the lamp off
#[no_mangle]
pub extern "C" fn power_off(ws_ptr: *mut WebSocket) -> bool {
    let ws = unsafe { ws_from_ptr(ws_ptr) };
    ws_write(ws, Command::PowerDeviceOff.into()).is_ok()
}

// frees the memory allocated for a websocket
#[no_mangle]
pub extern "C" fn free_websocket(ws: *mut WebSocket) {
    if !ws.is_null() {
        unsafe {
            Box::from_raw(ws);
        }
    }
}
