use hippo_core::SongDb;
use pancurses::Window;

#[derive(Default)]
pub struct SongWindow {
    url: String,
    title: String,
    type_name: String,
    authoring_tool: String,
    artist: String,
    date: String,
    message_raw: String,
    samples: Vec<String>,
    instrumets: Vec<String>,
}

impl SongWindow {
    pub fn new() -> SongWindow {
        SongWindow::default()
    }

    ///
    /// update the song window with the current meta data
    ///
    pub fn update_meta(&mut self, url: &str, song_db: &SongDb) {
        self.url = url.to_owned();
        self.title = song_db.get_key(url, 0, "title").unwrap_or(String::new());
        self.type_name = song_db.get_key(url, 0, "type_long").unwrap_or(String::new());
        self.authoring_tool = song_db.get_key(url, 0, "authoring_tool ").unwrap_or(String::new());
        self.artist = song_db.get_key(url, 0, "artist").unwrap_or(String::new());
        self.date = song_db.get_key(url, 0, "date").unwrap_or(String::new());
        self.message_raw = song_db.get_key(url, 0, "message_raw").unwrap_or(String::new());

        let mut counter = 1;
        let mut samples = Vec::new();
        let mut instrumets = Vec::new();

        // Get the samples

        loop {
            if let Some(sample) = song_db.get_key(url, 0, &format!("sample_{:04}", counter)) {
                samples.push(sample.to_owned());
            } else {
                break;
            }

            counter += 1;
        }

        self.samples = samples;

        // Get instrumets

        counter = 1;

        loop {
            if let Some(sample) = song_db.get_key(url, 0, &format!("instrument_{:04}", counter )) {
                instrumets .push(sample.to_owned());
            } else {
                break;
            }

            counter += 1;
        }

        self.instrumets = instrumets;
    }

    ///
    /// Draw a string to the screen and validate it contains some data
    ///
    fn draw_string(title: &str, data: &str, win: &Window, x_pos: i32, y_pos: i32) -> i32 {
        if data.is_empty() {
            return y_pos;
        }

        // TODO: Remove allocs
        win.mvaddstr(y_pos, x_pos, &format!("{} {}", title, data));

        y_pos + 1
    }

    ///
    /// Render the current info
    /// TODO: Do a subwindow?
    ///
    pub fn display(&self, window: &Window) {
        //let (width, height) = window.get_max_yx();

        window.clear();
        window.draw_box('|', '-');

        let mut y_pos = 2i32;
        let mut x_pos = 2i32;

        y_pos = Self::draw_string("Filename.........:", &self.url, window, x_pos, y_pos);
        y_pos = Self::draw_string("Title............:", &self.title, window, x_pos, y_pos);
        y_pos = Self::draw_string("Type.............:", &self.type_name, window, x_pos, y_pos);
        y_pos = Self::draw_string("Authoring tool...:", &self.authoring_tool, window, x_pos, y_pos);
        y_pos = Self::draw_string("Artist...........:", &self.artist, window, x_pos, y_pos);
        y_pos = Self::draw_string("Date.............:", &self.date, window, x_pos, y_pos);
        y_pos = Self::draw_string("Message..........:", &self.message_raw, window, x_pos, y_pos);

        x_pos = 20;

        if !self.samples.is_empty() {
            window.mvaddstr(y_pos, 2, "Samples..........:");
            y_pos += 1;

            for sample in &self.samples {
                window.mvaddstr(y_pos, x_pos, &sample);
                y_pos += 1;
            }
        }

        if !self.instrumets.is_empty() {
            window.mvaddstr(y_pos, 2, "Instrumets......:");
            y_pos += 1;

            for sample in &self.samples {
                window.mvaddstr(y_pos, x_pos, &sample);
                y_pos += 1;
            }
        }
    }
}
