use crate::sys;

#[repr(i32)]
#[derive(Clone, Copy, PartialEq, Eq)]
pub enum Error {
    Generic = sys::MA_ERROR,
    InvalidArgs = sys::MA_INVALID_ARGS,
    InvalidOperation = sys::MA_INVALID_OPERATION,
    OutOfMemory = sys::MA_OUT_OF_MEMORY,
    OutOfRange = sys::MA_OUT_OF_RANGE,
    AccessDenied = sys::MA_ACCESS_DENIED,
    DoesNotExist = sys::MA_DOES_NOT_EXIST,
    AlreadyExists = sys::MA_ALREADY_EXISTS,
    TooManyOpenFiles = sys::MA_TOO_MANY_OPEN_FILES,
    InvalidFile = sys::MA_INVALID_FILE,
    TooBig = sys::MA_TOO_BIG,
    PathTooLong = sys::MA_PATH_TOO_LONG,
    NameTooLong = sys::MA_NAME_TOO_LONG,
    NotDirectory = sys::MA_NOT_DIRECTORY,
    IsDirectory = sys::MA_IS_DIRECTORY,
    DirectoryNotEmpty = sys::MA_DIRECTORY_NOT_EMPTY,
    EndOfFile = sys::MA_END_OF_FILE,
    NoSpace = sys::MA_NO_SPACE,
    Busy = sys::MA_BUSY,
    IoError = sys::MA_IO_ERROR,
    Interrupt = sys::MA_INTERRUPT,
    Unavailable = sys::MA_UNAVAILABLE,
    AlreadyInUse = sys::MA_ALREADY_IN_USE,
    BadAddress = sys::MA_BAD_ADDRESS,
    BadSeek = sys::MA_BAD_SEEK,
    BadPipe = sys::MA_BAD_PIPE,
    Deadlock = sys::MA_DEADLOCK,
    TooManyLinks = sys::MA_TOO_MANY_LINKS,
    NotImplemented = sys::MA_NOT_IMPLEMENTED,
    NoMessage = sys::MA_NO_MESSAGE,
    BadMessage = sys::MA_BAD_MESSAGE,
    NoDataAvailable = sys::MA_NO_DATA_AVAILABLE,
    InvalidData = sys::MA_INVALID_DATA,
    Timeout = sys::MA_TIMEOUT,
    NoNetwork = sys::MA_NO_NETWORK,
    NotUnique = sys::MA_NOT_UNIQUE,
    NotSocket = sys::MA_NOT_SOCKET,
    NoAddress = sys::MA_NO_ADDRESS,
    BadProtocol = sys::MA_BAD_PROTOCOL,
    ProtocolUnavailable = sys::MA_PROTOCOL_UNAVAILABLE,
    ProtocolNotSupported = sys::MA_PROTOCOL_NOT_SUPPORTED,
    ProtocolFamilyNotSupported = sys::MA_PROTOCOL_FAMILY_NOT_SUPPORTED,
    AddressFamilyNotSupported = sys::MA_ADDRESS_FAMILY_NOT_SUPPORTED,
    SocketNotSupported = sys::MA_SOCKET_NOT_SUPPORTED,
    ConnectionReset = sys::MA_CONNECTION_RESET,
    AlreadyConnected = sys::MA_ALREADY_CONNECTED,
    NotConnected = sys::MA_NOT_CONNECTED,
    ConnectionRefused = sys::MA_CONNECTION_REFUSED,
    NoHost = sys::MA_NO_HOST,
    InProgress = sys::MA_IN_PROGRESS,
    Cancelled = sys::MA_CANCELLED,
    MemoryAlreadyMapped = sys::MA_MEMORY_ALREADY_MAPPED,
    AtEnd = sys::MA_AT_END,

    /* General miniaudio-specific errors. */
    FormatNotSupported = sys::MA_FORMAT_NOT_SUPPORTED,
    DeviceTypeNotSupported = sys::MA_DEVICE_TYPE_NOT_SUPPORTED,
    ShareModeNotSupported = sys::MA_SHARE_MODE_NOT_SUPPORTED,
    NoBackend = sys::MA_NO_BACKEND,
    NoDevice = sys::MA_NO_DEVICE,
    ApiNotFound = sys::MA_API_NOT_FOUND,
    InvalidDeviceConfig = sys::MA_INVALID_DEVICE_CONFIG,

    /* State errors. */
    DeviceNotInitialized = sys::MA_DEVICE_NOT_INITIALIZED,
    DeviceAlreadyInitialized = sys::MA_DEVICE_ALREADY_INITIALIZED,
    DeviceNotStarted = sys::MA_DEVICE_NOT_STARTED,
    DeviceNotStopped = sys::MA_DEVICE_NOT_STOPPED,

    /* Operation errors. */
    FailedToInitBackend = sys::MA_FAILED_TO_INIT_BACKEND,
    FailedToOpenBackendDevice = sys::MA_FAILED_TO_OPEN_BACKEND_DEVICE,
    FailedToStartBackendDevice = sys::MA_FAILED_TO_START_BACKEND_DEVICE,
    FailedToStopBackendDevice = sys::MA_FAILED_TO_STOP_BACKEND_DEVICE,
}

impl Error {
    #[inline(always)]
    pub(crate) const fn is_c_error(c_result: sys::ma_result) -> bool {
        c_result != sys::MA_SUCCESS as _
    }

    /// Converts a C result value into a `Result` with `Error`.
    pub(crate) fn from_c_result(c_result: sys::ma_result) -> Result<(), Error> {
        if !Self::is_c_error(c_result) {
            Ok(())
        } else {
            Err(Self::from_c_error(c_result))
        }
    }

    /// Converts an error from C library's int format into an `Error` enum.
    pub(crate) fn from_c_error(c_error: sys::ma_result) -> Error {
        match c_error {
            sys::MA_INVALID_ARGS => Error::InvalidArgs,
            sys::MA_INVALID_OPERATION => Error::InvalidOperation,
            sys::MA_OUT_OF_MEMORY => Error::OutOfMemory,
            sys::MA_OUT_OF_RANGE => Error::OutOfRange,
            sys::MA_ACCESS_DENIED => Error::AccessDenied,
            sys::MA_DOES_NOT_EXIST => Error::DoesNotExist,
            sys::MA_ALREADY_EXISTS => Error::AlreadyExists,
            sys::MA_TOO_MANY_OPEN_FILES => Error::TooManyOpenFiles,
            sys::MA_INVALID_FILE => Error::InvalidFile,
            sys::MA_TOO_BIG => Error::TooBig,
            sys::MA_PATH_TOO_LONG => Error::PathTooLong,
            sys::MA_NAME_TOO_LONG => Error::NameTooLong,
            sys::MA_NOT_DIRECTORY => Error::NotDirectory,
            sys::MA_IS_DIRECTORY => Error::IsDirectory,
            sys::MA_DIRECTORY_NOT_EMPTY => Error::DirectoryNotEmpty,
            sys::MA_END_OF_FILE => Error::EndOfFile,
            sys::MA_NO_SPACE => Error::NoSpace,
            sys::MA_BUSY => Error::Busy,
            sys::MA_IO_ERROR => Error::IoError,
            sys::MA_INTERRUPT => Error::Interrupt,
            sys::MA_UNAVAILABLE => Error::Unavailable,
            sys::MA_ALREADY_IN_USE => Error::AlreadyInUse,
            sys::MA_BAD_ADDRESS => Error::BadAddress,
            sys::MA_BAD_SEEK => Error::BadSeek,
            sys::MA_BAD_PIPE => Error::BadPipe,
            sys::MA_DEADLOCK => Error::Deadlock,
            sys::MA_TOO_MANY_LINKS => Error::TooManyLinks,
            sys::MA_NOT_IMPLEMENTED => Error::NotImplemented,
            sys::MA_NO_MESSAGE => Error::NoMessage,
            sys::MA_BAD_MESSAGE => Error::BadMessage,
            sys::MA_NO_DATA_AVAILABLE => Error::NoDataAvailable,
            sys::MA_INVALID_DATA => Error::InvalidData,
            sys::MA_TIMEOUT => Error::Timeout,
            sys::MA_NO_NETWORK => Error::NoNetwork,
            sys::MA_NOT_UNIQUE => Error::NotUnique,
            sys::MA_NOT_SOCKET => Error::NotSocket,
            sys::MA_NO_ADDRESS => Error::NoAddress,
            sys::MA_BAD_PROTOCOL => Error::BadProtocol,
            sys::MA_PROTOCOL_UNAVAILABLE => Error::ProtocolUnavailable,
            sys::MA_PROTOCOL_NOT_SUPPORTED => Error::ProtocolNotSupported,
            sys::MA_PROTOCOL_FAMILY_NOT_SUPPORTED => Error::ProtocolFamilyNotSupported,
            sys::MA_ADDRESS_FAMILY_NOT_SUPPORTED => Error::AddressFamilyNotSupported,
            sys::MA_SOCKET_NOT_SUPPORTED => Error::SocketNotSupported,
            sys::MA_CONNECTION_RESET => Error::ConnectionReset,
            sys::MA_ALREADY_CONNECTED => Error::AlreadyConnected,
            sys::MA_NOT_CONNECTED => Error::NotConnected,
            sys::MA_CONNECTION_REFUSED => Error::ConnectionRefused,
            sys::MA_NO_HOST => Error::NoHost,
            sys::MA_IN_PROGRESS => Error::InProgress,
            sys::MA_CANCELLED => Error::Cancelled,
            sys::MA_MEMORY_ALREADY_MAPPED => Error::MemoryAlreadyMapped,
            sys::MA_AT_END => Error::AtEnd,

            /* General miniaudio-specific errors. */
            sys::MA_FORMAT_NOT_SUPPORTED => Error::FormatNotSupported,
            sys::MA_DEVICE_TYPE_NOT_SUPPORTED => Error::DeviceTypeNotSupported,
            sys::MA_SHARE_MODE_NOT_SUPPORTED => Error::ShareModeNotSupported,
            sys::MA_NO_BACKEND => Error::NoBackend,
            sys::MA_NO_DEVICE => Error::NoDevice,
            sys::MA_API_NOT_FOUND => Error::ApiNotFound,
            sys::MA_INVALID_DEVICE_CONFIG => Error::InvalidDeviceConfig,

            /* State errors. */
            sys::MA_DEVICE_NOT_INITIALIZED => Error::DeviceNotInitialized,
            sys::MA_DEVICE_ALREADY_INITIALIZED => Error::DeviceAlreadyInitialized,
            sys::MA_DEVICE_NOT_STARTED => Error::DeviceNotStarted,
            sys::MA_DEVICE_NOT_STOPPED => Error::DeviceNotStopped,

            /* Operation errors. */
            sys::MA_FAILED_TO_INIT_BACKEND => Error::FailedToInitBackend,
            sys::MA_FAILED_TO_OPEN_BACKEND_DEVICE => Error::FailedToOpenBackendDevice,
            sys::MA_FAILED_TO_START_BACKEND_DEVICE => Error::FailedToStartBackendDevice,
            sys::MA_FAILED_TO_STOP_BACKEND_DEVICE => Error::FailedToStopBackendDevice,
            _ => Error::Generic,
        }
    }

    /// Returns the message associated with an error type.
    pub fn message(self) -> &'static str {
        match self {
            Error::Generic => "generic",
            Error::InvalidArgs => "invalid args",
            Error::InvalidOperation => "invalid operation",
            Error::OutOfMemory => "out of memory",
            Error::OutOfRange => "out of range",
            Error::AccessDenied => "access denied",
            Error::DoesNotExist => "does not exist",
            Error::AlreadyExists => "already exists",
            Error::TooManyOpenFiles => "too many open files",
            Error::InvalidFile => "invalid file",
            Error::TooBig => "too big",
            Error::PathTooLong => "path too long",
            Error::NameTooLong => "name too long",
            Error::NotDirectory => "not directory",
            Error::IsDirectory => "is directory",
            Error::DirectoryNotEmpty => "directory not empty",
            Error::EndOfFile => "end of file",
            Error::NoSpace => "no space",
            Error::Busy => "busy",
            Error::IoError => "io error",
            Error::Interrupt => "interrupt",
            Error::Unavailable => "unavailable",
            Error::AlreadyInUse => "already in use",
            Error::BadAddress => "bad address",
            Error::BadSeek => "bad seek",
            Error::BadPipe => "bad pipe",
            Error::Deadlock => "deadlock",
            Error::TooManyLinks => "too many links",
            Error::NotImplemented => "not implemented",
            Error::NoMessage => "no message",
            Error::BadMessage => "bad message",
            Error::NoDataAvailable => "no data available",
            Error::InvalidData => "invalid data",
            Error::Timeout => "timeout",
            Error::NoNetwork => "no network",
            Error::NotUnique => "not unique",
            Error::NotSocket => "not socket",
            Error::NoAddress => "no address",
            Error::BadProtocol => "bad protocol",
            Error::ProtocolUnavailable => "protocol unavailable",
            Error::ProtocolNotSupported => "protocol not supported",
            Error::ProtocolFamilyNotSupported => "protocol family not supported",
            Error::AddressFamilyNotSupported => "address family not supported",
            Error::SocketNotSupported => "socket not supported",
            Error::ConnectionReset => "connection reset",
            Error::AlreadyConnected => "already connected",
            Error::NotConnected => "not connected",
            Error::ConnectionRefused => "connection refused",
            Error::NoHost => "no host",
            Error::InProgress => "in progress",
            Error::Cancelled => "cancelled",
            Error::MemoryAlreadyMapped => "memory already mapped",
            Error::AtEnd => "at end",

            /* General miniaudio-specific errors. */
            Error::FormatNotSupported => "format not supported",
            Error::DeviceTypeNotSupported => "device type not supported",
            Error::ShareModeNotSupported => "share mode not supported",
            Error::NoBackend => "no backend",
            Error::NoDevice => "no device",
            Error::ApiNotFound => "api not found",
            Error::InvalidDeviceConfig => "invalid device config",

            /* State errors. */
            Error::DeviceNotInitialized => "device not initialized",
            Error::DeviceAlreadyInitialized => "device already initialized",
            Error::DeviceNotStarted => "device not started",
            Error::DeviceNotStopped => "device not stopped",

            /* Operation errors. */
            Error::FailedToInitBackend => "failed to init backend",
            Error::FailedToOpenBackendDevice => "failed to open backend device",
            Error::FailedToStartBackendDevice => "failed to start backend device",
            Error::FailedToStopBackendDevice => "failed to stop backend device",
        }
    }
}

impl std::fmt::Display for Error {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", self.message())
    }
}

impl std::fmt::Debug for Error {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", self.message())
    }
}

