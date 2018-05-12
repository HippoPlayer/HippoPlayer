use service::MessageApi;

#[derive(Serialize)]
pub enum ListPosition {
	Start,
	End,
}

#[derive(Serialize)]
pub struct AddUrls {
	pub list_position: ListPosition,
	pub urls: Vec<String>,
}

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


