extern crate serde;

#[macro_use]
extern crate serde_derive;

extern crate rmp_serde as rmps;

extern crate rute;

pub mod messages;
pub mod ffi;
pub mod service;
pub mod view;

pub use service::*;
