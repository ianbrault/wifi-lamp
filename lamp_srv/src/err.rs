/*
** src/err.rs
*/

use std::fmt;
use std::io;
use std::net::TcpStream;

use tungstenite::ServerHandshake;
use tungstenite::handshake::server::NoCallback;

type HandshakeError = tungstenite::HandshakeError<ServerHandshake<TcpStream, NoCallback>>;

pub enum Error {
    Handshake(HandshakeError),
    IO(io::Error),
    Tungstenite(tungstenite::Error),
}

impl Error {
    // is this an error which is caused by the client closing the connection?
    pub fn closed_connection(&self) -> bool {
        match self {
            Self::IO(e) => e.kind() == io::ErrorKind::ConnectionAborted,
            Self::Tungstenite(e) => e == tungstenite::Error::ConnectionClosed,
            _ => false,
        }
    }
}

impl From<HandshakeError> for Error {
    fn from(err: HandshakeError) -> Self {
        Self::Handshake(err)
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
            Self::IO(e) => write!(f, "{}", e),
            Self::Handshake(e) => write!(f, "{}", e),
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
