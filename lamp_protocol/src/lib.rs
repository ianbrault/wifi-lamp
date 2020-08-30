/*
** src/lib.rs
*/

pub mod err;
pub use err::Error;

use std::convert::TryFrom;
use std::fmt;

use tungstenite::Message;

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

pub enum Command {
    DeclareClientType(ClientType, Owner),
    DeclareClientTypeAck,
}

impl Command {
    // opcode definitions
    const OP_DECL_CLI_TYPE: u8     = 0x10;
    const OP_DECL_CLI_TYPE_ACK: u8 = 0x11;

    fn opcode(&self) -> u8 {
        match self {
            Self::DeclareClientType(_,_) => Self::OP_DECL_CLI_TYPE,
            Self::DeclareClientTypeAck   => Self::OP_DECL_CLI_TYPE_ACK,
        }
    }

    pub fn name(&self) -> &'static str {
        match self {
            Self::DeclareClientType(_,_) => "DeclareClientType",
            Self::DeclareClientTypeAck   => "DeclareClientTypeAck",
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

    /*
    ** parse Command from raw bytes (from tungstenite::Message::Binary)
    */

    fn try_parse_declare_client_type(rest: &[u8]) -> Result<Self, err::Error> {
        match rest {
            [] => Err(err::Error::invalid_command(
                format!(
                    "incomplete DeclareClientType command ({:?}): missing client type, owner arguments",
                    rest))),
            [_] => Err(err::Error::invalid_command(
                format!(
                    "incomplete DeclareClientType command ({:?}): missing owner argument",
                    rest))),
            [b_client_type, b_owner] => {
                let client_type = ClientType::try_from(*b_client_type)?;
                let owner = Owner::try_from(*b_owner)?;
                Ok(Self::DeclareClientType(client_type, owner))
            },
            _ => Err(err::Error::invalid_command(
                format!(
                    "invalid DeclareClientType command ({:?}): too many arguments",
                    rest)))
        }
    }

    fn try_parse_declare_client_type_ack(_rest: &[u8]) -> Result<Self, err::Error> {
        Ok(Self::DeclareClientTypeAck)
    }

    fn try_parse_bytes(bytes: Vec<u8>) -> Result<Self, err::Error> {
        if let [opcode, rest @ ..] = bytes.as_slice() {
            match *opcode {
                Self::OP_DECL_CLI_TYPE => Self::try_parse_declare_client_type(rest),
                Self::OP_DECL_CLI_TYPE_ACK => Self::try_parse_declare_client_type_ack(rest),
                _ => Err(err::Error::invalid_opcode(*opcode)),
            }
        } else {
            Err(err::Error::invalid_command("empty message"))
        }
    }
}

impl Into<Message> for Command {
    fn into(self) -> Message {
        let opcode = self.opcode();
        let bytes = match self {
            Self::DeclareClientType(client_type, owner) => vec![
                opcode, client_type.into(), owner.into()],
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
            _ => Err(Self::Error::invalid_command(format!("invalid client type {:#04x}", byte)))
        }
    }
}

pub enum Owner {
    Arni,
    Ian,
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
            _ => Err(Self::Error::invalid_command(format!("invalid owner {:#04x}", byte)))
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
** unit tests coming... soon???
*/
