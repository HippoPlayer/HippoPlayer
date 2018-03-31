use wrui::wrui::{Widget, Label, LineEdit, PlainTextEdit, VBoxLayout, FramelessWindow, ListWidgetItem, ListWidget };
use wrui::Ui;
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
    wrui: Ui,
}
impl SongInfoView {
    fn create_label_text(wrui: Ui, label_name: &str, parent_layout: &VBoxLayout) -> LineEdit {
        let layout = wrui.create_h_box_layout();

        let label_text = wrui.create_label();
        let line = wrui.create_line_edit();

        //line.set_read_only(true);
        label_text.set_text(label_name);

        layout.add_widget(&label_text);
        layout.add_widget(&line);

        parent_layout.add_layout(&layout);

        line
    }

    fn create_label_plain_text(wrui: Ui, label_name: &str, parent_layout: &VBoxLayout) -> PlainTextEdit {
        let layout = wrui.create_h_box_layout();

        let label_text = wrui.create_label();
        let line = wrui.create_plain_text_edit();

        //line.set_read_only(true);
        label_text.set_text(label_name);

        layout.add_widget(&label_text);
        layout.add_widget(&line);

        parent_layout.add_layout(&layout);

        line
    }

    pub fn new(wrui: Ui) -> SongInfoView {
        //let view = wrui.create_frameless_window();
        let view = wrui.create_widget();

        let main_layout = wrui.create_v_box_layout();

        // basic line 

        let title = Self::create_label_text(wrui, "Title", &main_layout);
        let artist = Self::create_label_text(wrui, "Artist", &main_layout);
        let song_type = Self::create_label_text(wrui, "Type", &main_layout);
        let authoring_tool = Self::create_label_text(wrui, "Tool", &main_layout);
        let date = Self::create_label_text(wrui, "Date", &main_layout);

        // message box

        let message = Self::create_label_plain_text(wrui, "Message", &main_layout);
        let samples = Self::create_label_plain_text(wrui, "Samples", &main_layout);
        let instruments = Self::create_label_plain_text(wrui, "Instruments", &main_layout);

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
            wrui,
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
        self.song_type.set_text(&song_type);
        self.artist.set_text(&artist);
        self.date.set_text(&date);
        self.message.set_plain_text(&message);

        Self::fill_vary(&self.samples, resource, "sample", song_db);
        Self::fill_vary(&self.instruments, resource, "instrument", song_db);
    }
}

