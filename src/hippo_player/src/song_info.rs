use rute::rute::{Widget, LineEdit, PlainTextEdit, VBoxLayout };
use rute::Ui;
use song_db::SongDb;

pub struct SongInfoView {
    pub view: Widget,
    pub title: LineEdit,
    pub song_type: LineEdit,
    pub authoring_tool: LineEdit,
    pub artist: LineEdit,
    pub date: LineEdit,
    pub message: PlainTextEdit,
    pub samples: PlainTextEdit,
    pub instruments: PlainTextEdit,
    _rute: Ui,
}
impl SongInfoView {
    fn create_label_text(rute: Ui, label_name: &str, parent_layout: &VBoxLayout) -> LineEdit {
        let layout = rute.create_h_box_layout();

        let label_text = rute.create_label();
        let line = rute.create_line_edit();

        //line.set_read_only(true);
        label_text.set_text(label_name);

        layout.add_widget(&label_text);
        layout.add_widget(&line);

        parent_layout.add_layout(&layout);

        line
    }

    fn create_label_plain_text(rute: Ui, label_name: &str, parent_layout: &VBoxLayout) -> PlainTextEdit {
        let layout = rute.create_h_box_layout();

        let label_text = rute.create_label();
        let line = rute.create_plain_text_edit();

        //line.set_read_only(true);
        label_text.set_text(label_name);

        layout.add_widget(&label_text);
        layout.add_widget(&line);

        parent_layout.add_layout(&layout);

        line
    }

    pub fn new(rute: Ui) -> SongInfoView {
        //let view = rute.create_frameless_window();
        let view = rute.create_widget();

        let main_layout = rute.create_v_box_layout();

        // basic line

        let title = Self::create_label_text(rute, "Title", &main_layout);
        let artist = Self::create_label_text(rute, "Artist", &main_layout);
        let song_type = Self::create_label_text(rute, "Type", &main_layout);
        let authoring_tool = Self::create_label_text(rute, "Tool", &main_layout);
        let date = Self::create_label_text(rute, "Date", &main_layout);

        // message box

        let message = Self::create_label_plain_text(rute, "Message", &main_layout);
        let samples = Self::create_label_plain_text(rute, "Samples", &main_layout);
        let instruments = Self::create_label_plain_text(rute, "Instruments", &main_layout);

        view.set_layout(&main_layout);

        SongInfoView {
            view,
            title,
            song_type,
            authoring_tool,
            artist,
            date,
            message,
            samples,
            instruments,
            _rute: rute,
        }
    }

    pub fn show(&self) {
        self.view.show();
    }

    pub fn get_value(resource: &str, key: &str, song_db: &SongDb) -> String {
        match song_db.get_key(resource, 0, key) {
            None => "".to_owned(),
            Some(text) => text.to_owned(),
        }
    }

    pub fn fill_vary(view: &PlainTextEdit, resource: &str, init_val: &str, song_db: &SongDb) {
        view.clear();
        let mut count = 1;

        loop {
            let s = format!("{}_{:04}", init_val, count);

            match song_db.get_key(resource, 0, &s) {
                None => break,
                Some(text) => view.append_plain_text(&text),
            }

            count += 1;
        }
    }

    pub fn update_data(&mut self, resource: &str, song_db: &SongDb) {
        let title = Self::get_value(resource, "title", song_db);
        let song_type = Self::get_value(resource, "type", song_db);
        let tool = Self::get_value(resource, "authoring_tool", song_db);
        let artist = Self::get_value(resource, "artist", song_db);
        let date = Self::get_value(resource, "date", song_db);
        let message = Self::get_value(resource, "message", song_db);

        self.title.set_text(&title);
        self.authoring_tool.set_text(&tool);
        self.song_type.set_text(&song_type);
        self.artist.set_text(&artist);
        self.date.set_text(&date);
        self.message.set_plain_text(&message);

        Self::fill_vary(&self.samples, resource, "sample", song_db);
        Self::fill_vary(&self.instruments, resource, "instrument", song_db);
    }
}

