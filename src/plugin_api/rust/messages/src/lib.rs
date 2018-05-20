#[macro_use]
extern crate serde_derive;
extern crate serde;
extern crate num_integer;

pub extern crate rmp as msgpack;
extern crate rmp_serde as rmps;

pub enum MsgType {
    Request,
    Response,
    Notification,
}

#[derive(Debug, Serialize, Deserialize)]
pub enum ListPosition {
	Start,
	End,
}

#[derive(Debug, Serialize, Deserialize)]
pub struct AddUrls {
	pub list_position: ListPosition,
	pub urls: Vec<String>,
}

impl AddUrls {
	pub fn new() -> AddUrls {
		AddUrls {
			list_position: ListPosition::End,
			urls: Vec::new(),
		}
	}
}

pub use msgpack::encode::ValueWriteError as ValueWriteError;
pub use msgpack::decode::NumValueReadError as NumValueReadError;

pub use encode::Message as MessageEncode;
pub use decode::Message as MessageDecode;

pub mod decode;
pub mod encode;

/*
impl MessageApi {
	pub fn next_song(&self) {
		println!("Requesting next song");
		let msg = self.begin_notification("hippo_playlist_next_song");
		msg.write_uint(0);
		self.end_message(msg)
	}

	pub fn add_urls(&self, urls_msg: AddUrls) -> u32 {
		self.send_request("hippo_playlist_add_urls", &urls_msg)
	}
}
*/



