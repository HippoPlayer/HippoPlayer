#[macro_use]
extern crate serde_derive;
extern crate num_traits;
extern crate serde;

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

/*
#[derive(Debug, Serialize, Deserialize)]
pub struct LoadedUrls {
    //#[serde(borrow)]
    //pub urls: Vec<(&'a str, &'a str)>,
    pub urls: Vec<(String, String)>,
}

impl AddUrls {
    pub fn new() -> AddUrls {
        AddUrls {
            list_position: ListPosition::End,
            urls: Vec::new(),
        }
    }
}

impl LoadedUrls {
    pub fn new() -> LoadedUrls {
        LoadedUrls { urls: Vec::new() }
    }
}
*/

pub use msgpack::decode::NumValueReadError;
pub use msgpack::encode::ValueWriteError;

pub use decode::Message as MessageDecode;
pub use encode::Message as MessageEncode;

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