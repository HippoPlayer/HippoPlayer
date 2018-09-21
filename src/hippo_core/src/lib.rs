extern crate hippo_api;
extern crate messages;
extern crate rodio;
extern crate dynamic_reload;
extern crate walkdir;

#[macro_use]
extern crate serde_derive;
pub extern crate rmp;
extern crate rmp_rpc;
extern crate serde;
extern crate serde_json;

extern crate rmp_serde as rmps;

pub mod audio;
pub mod playlist;
pub mod plugin_handler;
pub mod song_db;
pub mod service;
pub mod service_ffi;

pub use audio::*;
pub use plugin_handler::*;
pub use playlist::*;
pub use song_db::*;
pub use service::*;
pub use service_ffi::*;
