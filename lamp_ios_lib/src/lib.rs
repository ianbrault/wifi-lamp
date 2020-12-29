/*
** src/lib.rs
*/

use std::convert::TryFrom;
use std::ffi::{c_void, CStr};
use std::marker::PhantomData;
use std::os::raw::c_char;
use std::ptr;

use lamp_protocol::{ws_read, ws_write, ClientType, Command, Error, Owner};
use tungstenite::client::AutoStream;

#[repr(C)]
pub struct WebSocket<T> {
    inner: *mut c_void,
    // cbindgen will complain that WebSocket needs to be generic, even though
    // we aren't using the generic type
    phantom: PhantomData<T>,
}

impl<T> WebSocket<T> {
    fn null() -> Self {
        Self {
            inner: ptr::null_mut(),
            phantom: PhantomData,
        }
    }

    fn is_null(&self) -> bool {
        self.inner.is_null()
    }

    fn into_websocket(&self) -> &mut tungstenite::WebSocket<T> {
        // WARNING: ensure that this call is guarded by a null check
        assert!(!self.is_null());
        unsafe { &mut *(self.inner as *mut tungstenite::WebSocket<T>) }
    }
}

impl<T> From<tungstenite::WebSocket<T>> for WebSocket<T> {
    fn from(socket: tungstenite::WebSocket<T>) -> Self {
        Self {
            inner: Box::into_raw(Box::new(socket)) as *mut c_void,
            phantom: PhantomData,
        }
    }
}

// connect to the server and open a websocket
#[no_mangle]
pub unsafe extern "C" fn connect_to_lamp(address: *const c_char) -> WebSocket<AutoStream> {
    // convert the provided address into a Rust-friendly string
    assert!(!address.is_null());
    let address = match CStr::from_ptr(address).to_str() {
        Ok(s) => s,
        Err(_) => return WebSocket::null(),
    };

    // connect to the provided address
    match tungstenite::connect(address) {
        Ok((websocket, _)) => WebSocket::from(websocket),
        Err(_) => WebSocket::null(),
    }
}

// disconnect from the server and free memory allocated for the websocket
#[no_mangle]
pub unsafe extern "C" fn disconnect_from_lamp(socket: WebSocket<AutoStream>) {
    if !socket.is_null() {
        let ws = socket.into_websocket();
        let _ = ws.close(None);
        // box up the websocket pointer so that the memory can be freed
        let _ = Box::from_raw(socket.inner);
    }
}

fn declare_client_connection_inner(
    ws: &mut tungstenite::WebSocket<AutoStream>,
    owner: Owner,
) -> Result<(), Error> {
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
#[no_mangle]
pub extern "C" fn declare_client_connection(socket: WebSocket<AutoStream>, owner: u8) -> bool {
    if socket.is_null() {
        return false;
    }

    let ws = socket.into_websocket();
    let owner = match Owner::try_from(owner) {
        Ok(owner) => owner,
        Err(_) => return false,
    };
    declare_client_connection_inner(ws, owner).is_ok()
}

// turn the lamp on
#[no_mangle]
pub extern "C" fn power_on(socket: WebSocket<AutoStream>) -> bool {
    if socket.is_null() {
        return false;
    }

    let ws = socket.into_websocket();
    ws_write(ws, Command::PowerDeviceOn.into()).is_ok()
}

// turn the lamp off
#[no_mangle]
pub extern "C" fn power_off(socket: WebSocket<AutoStream>) -> bool {
    if socket.is_null() {
        return false;
    }

    let ws = socket.into_websocket();
    ws_write(ws, Command::PowerDeviceOff.into()).is_ok()
}
