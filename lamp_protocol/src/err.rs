/*
** src/err.rs
*/

use std::fmt;
use std::io;
use std::net::TcpStream;

use tungstenite::handshake::server::NoCallback;
use tungstenite::ServerHandshake;

type HandshakeError = tungstenite::HandshakeError<ServerHandshake<TcpStream, NoCallback>>;

pub enum Error {
    HandshakeError(HandshakeError),
    IO(io::Error),
    InvalidCommand(String),
    InvalidState(String),
    Protocol(String),
    Tungstenite(tungstenite::Error),
}

impl Error {
    pub fn invalid_command<S>(reason: S) -> Self
    where
        S: Into<String>,
    {
        Self::InvalidCommand(reason.into())
    }

    pub fn invalid_opcode(opcode: u8) -> Self {
        Self::InvalidCommand(format!("invalid opcode {:#04x}", opcode))
    }

    pub fn invalid_state(state: u8) -> Self {
        Self::InvalidState(format!("invalid state {:#04x}", state))
    }

    pub fn protocol_error<S>(reason: S) -> Self
    where
        S: Into<String>,
    {
        Self::Protocol(reason.into())
    }

    pub fn unexpected_command<S, T>(expected: S, found: T) -> Self
    where
        S: Into<String>,
        T: Into<String>,
    {
        Self::protocol_error(format!(
            "expected a {} command but received a {} command instead",
            expected.into(),
            found.into()
        ))
    }

    // is this an error which is caused by the client closing the connection?
    pub fn closed_connection(&self) -> bool {
        match self {
            Self::IO(e) => e.kind() == io::ErrorKind::ConnectionAborted,
            Self::Tungstenite(e) => matches!(e, tungstenite::Error::ConnectionClosed),
            _ => false,
        }
    }
}

impl From<HandshakeError> for Error {
    fn from(err: HandshakeError) -> Self {
        Self::HandshakeError(err)
    }
}

impl From<io::Error> for Error {
    fn from(err: io::Error) -> Self {
        Self::IO(err)
    }
}

impl From<tungstenite::Error> for Error {
    fn from(err: tungstenite::Error) -> Self {
        Self::Tungstenite(err)
    }
}

impl fmt::Display for Error {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Self::HandshakeError(e) => write!(f, "{}", e),
            Self::IO(e) => write!(f, "{}", e),
            Self::InvalidCommand(e) => write!(f, "{}", e),
            Self::InvalidState(e) => write!(f, "{}", e),
            Self::Protocol(e) => write!(f, "{}", e),
            Self::Tungstenite(e) => write!(f, "{}", e),
        }
    }
}

impl fmt::Debug for Error {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}", self)
    }
}

impl std::error::Error for Error {}
