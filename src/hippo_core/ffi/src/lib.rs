//pub extern crate messages;

#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]


#[cfg(debug_assertions)]
pub const DUMMY_STRING: &str = include_str!("../../../plugin_api/HippoPlugin.h");

include!(concat!(env!("OUT_DIR"), "/hippo_api.rs"));
