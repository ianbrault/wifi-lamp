/*
** src/lib.rs
*/

pub mod err;
pub use err::Error;

use std::convert::TryFrom;
use std::fmt;
use std::io::{Read, Write};
use std::sync::{Condvar, Mutex};

use log::debug;
use tungstenite::{Message, WebSocket};

/*
** TODO: better namespacing
*/

// helper function to print binary messages as hexadecimal bytes
pub fn message_as_hex(message: &Message) -> String {
    if let Message::Binary(bytes) = message {
        bytes.iter()
            .map(|b| format!("{:02x}", b))
            .collect::<Vec<String>>()
            .join(" ")
    } else {
        // only binary messages are supported, but fail silently
        String::new()
    }
}

/*
** convenience functions to read/write websockets
*/

pub fn ws_read<S>(websocket: &mut WebSocket<S>) -> Result<Message, Error>
where S: Read + Write
{
    let message = websocket.read_message()?;
    debug!("received message: {}", message_as_hex(&message));
    Ok(message)
}

pub fn ws_write<S>(websocket: &mut WebSocket<S>, message: Message) -> Result<(), Error>
where S: Read + Write
{
    debug!("sending message: {}", message_as_hex(&message));
    websocket.write_message(message)?;
    Ok(())
}

/*
** protocol state machine
*/

#[derive(Clone, Debug)]
pub enum State {
    NotConnected,
    ServerOffline,
    Off,
    OnWaiting,
    OnPaired,
}

impl State {
    pub fn is_on(&self) -> bool {
        match self {
            Self::OnWaiting | Self::OnPaired => true,
            _ => false,
        }
    }
}

impl Into<u8> for State {
    fn into(self) -> u8 {
        match self {
            Self::NotConnected  => 0x00,
            Self::ServerOffline => 0x01,
            Self::Off           => 0x02,
            Self::OnWaiting     => 0x03,
            Self::OnPaired      => 0x04,
        }
    }
}

impl TryFrom<u8> for State {
    type Error = err::Error;

    fn try_from(byte: u8) -> Result<Self, Self::Error> {
        match byte {
            0x00 => Ok(Self::NotConnected),
            0x01 => Ok(Self::ServerOffline),
            0x02 => Ok(Self::Off),
            0x03 => Ok(Self::OnWaiting),
            0x04 => Ok(Self::OnPaired),
            _    => Err(Error::invalid_state(byte))
        }
    }
}

/*
** protocol commands and associated structures
*/

pub enum Command {
    DeclareClientType(ClientType, Owner),
    DeclareClientTypeAck,
    DeviceStateChanged(State),
    PowerDeviceOff,
    PowerDeviceOn,
}

impl Command {
    // opcode definitions
    const OP_DECL_CLI_TYPE: u8     = 0x10;
    const OP_DECL_CLI_TYPE_ACK: u8 = 0x11;
    const OP_DEV_ST_CHANGED: u8    = 0x12;
    const OP_PWR_DEV_OFF: u8       = 0x13;
    const OP_PWR_DEV_ON: u8        = 0x14;

    fn opcode(&self) -> u8 {
        match self {
            Self::DeclareClientType(_,_) => Self::OP_DECL_CLI_TYPE,
            Self::DeclareClientTypeAck   => Self::OP_DECL_CLI_TYPE_ACK,
            Self::DeviceStateChanged(_)  => Self::OP_DEV_ST_CHANGED,
            Self::PowerDeviceOff         => Self::OP_PWR_DEV_OFF,
            Self::PowerDeviceOn          => Self::OP_PWR_DEV_ON,
        }
    }

    pub fn name(&self) -> &'static str {
        match self {
            Self::DeclareClientType(_,_) => "DeclareClientType",
            Self::DeclareClientTypeAck   => "DeclareClientTypeAck",
            Self::DeviceStateChanged(_)  => "DeviceStateChanged",
            Self::PowerDeviceOff         => "PowerDeviceOff",
            Self::PowerDeviceOn          => "PowerDeviceOn",
        }
    }

    // used to command devices
    pub fn is_device_command(&self) -> bool {
        match self {
            Self::PowerDeviceOff | Self::PowerDeviceOn => true,
            _ => false,
        }
    }

    /*
    ** create Command as tungstenite::Message
    */

    pub fn declare_client_type(client_type: ClientType, owner: Owner) -> Message {
        Self::DeclareClientType(client_type, owner).into()
    }

    pub fn declare_client_type_ack() -> Message {
        Self::DeclareClientTypeAck.into()
    }

    pub fn device_state_changed(state: State) -> Message {
        Self::DeviceStateChanged(state).into()
    }

    /*
    ** parse Command from raw bytes (from tungstenite::Message::Binary)
    */

    fn try_parse_declare_client_type(rest: &[u8]) -> Result<Self, Error> {
        match rest {
            [] => Err(Error::invalid_command(
                format!("incomplete DeclareClientType command ({:?}): missing client type, owner arguments",
                        rest))),
            [_] => Err(Error::invalid_command(
                format!("incomplete DeclareClientType command ({:?}): missing owner argument",
                        rest))),
            [b_client_type, b_owner] => {
                let client_type = ClientType::try_from(*b_client_type)?;
                let owner = Owner::try_from(*b_owner)?;
                Ok(Self::DeclareClientType(client_type, owner))
            },
            _ => Err(Error::invalid_command(
                format!("invalid DeclareClientType command ({:?}): too many arguments",
                        rest)))
        }
    }

    fn try_parse_device_state_changed(rest: &[u8]) -> Result<Self, Error> {
        match rest {
            [] => Err(Error::invalid_command(
                format!("incomplete DeviceStateChanged command ({:?}): missing state argument",
                        rest))),
            [b_state] => {
                let state = State::try_from(*b_state)?;
                Ok(Self::DeviceStateChanged(state))
            },
            _ => Err(Error::invalid_command(
                format!("invalid DeviceStateChanged command ({:?}): too many arguments",
                        rest)))
        }
    }

    fn try_parse_bytes(bytes: Vec<u8>) -> Result<Self, Error> {
        if let [opcode, rest @ ..] = bytes.as_slice() {
            match *opcode {
                Self::OP_DECL_CLI_TYPE => Self::try_parse_declare_client_type(rest),
                Self::OP_DECL_CLI_TYPE_ACK => Ok(Self::DeclareClientTypeAck),
                Self::OP_DEV_ST_CHANGED => Self::try_parse_device_state_changed(rest),
                Self::OP_PWR_DEV_OFF => Ok(Self::PowerDeviceOff),
                Self::OP_PWR_DEV_ON => Ok(Self::PowerDeviceOn),
                _ => Err(Error::invalid_opcode(*opcode)),
            }
        } else {
            Err(Error::invalid_command("empty message"))
        }
    }
}

impl Into<Message> for Command {
    fn into(self) -> Message {
        let opcode = self.opcode();
        let bytes = match self {
            Self::DeclareClientType(cli_type, owner) => vec![opcode, cli_type.into(), owner.into()],
            Self::DeviceStateChanged(state) => vec![opcode, state.into()],
            _ => vec![opcode],
        };
        Message::Binary(bytes)
    }
}

impl TryFrom<Message> for Command {
    type Error = err::Error;

    fn try_from(message: Message) -> Result<Self, Self::Error> {
        // commands must be in binary
        if let Message::Binary(bytes) = message {
            Self::try_parse_bytes(bytes)
        } else {
            Err(Self::Error::invalid_command("command messages must be in a binary format"))
        }
    }
}

pub enum ClientType {
    Device,
    User,
}

impl Into<u8> for ClientType {
    fn into(self) -> u8 {
        match self {
            Self::Device => 0x20,
            Self::User   => 0x21,
        }
    }
}

impl TryFrom<u8> for ClientType {
    type Error = err::Error;

    fn try_from(byte: u8) -> Result<Self, Self::Error> {
        match byte {
            0x20 => Ok(Self::Device),
            0x21 => Ok(Self::User),
            _    => Err(Self::Error::invalid_command(format!("invalid client type {:#04x}", byte))),
        }
    }
}

#[derive(Clone, Copy)]
pub enum Owner {
    Arni,
    Ian,
}

impl Owner {
    pub fn partner(&self) -> Self {
        match self {
            Self::Arni => Self::Ian,
            Self::Ian  => Self::Arni,
        }
    }
}

impl Into<u8> for Owner {
    fn into(self) -> u8 {
        match self {
            Self::Arni => 0x30,
            Self::Ian  => 0x31,
        }
    }
}

impl TryFrom<u8> for Owner {
    type Error = err::Error;

    fn try_from(byte: u8) -> Result<Self, Self::Error> {
        match byte {
            0x30 => Ok(Self::Arni),
            0x31 => Ok(Self::Ian),
            _    => Err(Self::Error::invalid_command(format!("invalid owner {:#04x}", byte))),
        }
    }
}

impl fmt::Display for Owner {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Self::Arni => write!(f, "Arni"),
            Self::Ian => write!(f, "Ian"),
        }
    }
}

/*
** wraps the functionality of a Condvar used purely for notifications
*/

pub struct Notifier {
    condvar: Condvar,
    mutex: Mutex<()>,
}

impl Notifier {
    pub fn new() -> Self {
        Self { condvar: Condvar::new(), mutex: Mutex::new(()) }
    }

    pub fn wait(&self) {
        let guard = self.mutex.lock().unwrap();
        let _ = self.condvar.wait(guard).unwrap();
    }

    pub fn notify_all(&self) {
        self.condvar.notify_all()
    }
}

/*
** unit tests coming... soon???
*/
