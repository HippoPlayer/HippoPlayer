#[macro_use]
extern crate hippo_api;

//#[macro_use]
extern crate rute;

use rute::PluginUi;

// TODO: Fix me
//use std::os::raw::c_void;

use rute::rute::{LineEdit, PlainTextEdit, VBoxLayout};
use hippo_api::view::View;
use hippo_api::service::Service;

#[derive(Default)]
struct SongInfo {
    title: LineEdit,
    song_type: LineEdit,
    authoring_tool: LineEdit,
    artist: LineEdit,
    date: LineEdit,
    message: PlainTextEdit,
    samples: PlainTextEdit,
    instruments: PlainTextEdit,
    ui: PluginUi,
}

impl View for SongInfo {
    fn new(_service: &Service) -> SongInfo {
        SongInfo::default()
    }

    fn setup_ui(&mut self, ui: PluginUi) {
        let view = ui.get_parent();
        let main_layout = ui.create_v_box_layout();

        self.title = Self::create_label_text(ui, "Title", &main_layout);
        self.song_type = Self::create_label_text(ui, "Type", &main_layout);
        self.authoring_tool = Self::create_label_text(ui, "Tool", &main_layout);
        self.artist = Self::create_label_text(ui, "Artist", &main_layout);
        self.date = Self::create_label_text(ui, "Date", &main_layout);
        self.message = Self::create_label_plain_text(ui, "Message", &main_layout);
        self.samples = Self::create_label_plain_text(ui, "Samples", &main_layout);
        self.instruments = Self::create_label_plain_text(ui, "Instruments", &main_layout);
        self.ui = ui;

        view.set_layout(&main_layout);
    }

    fn destroy(&mut self) {

    }
}

impl SongInfo {
    ///
    /// Create a label + single line text
    ///
    fn create_label_text(rute: PluginUi, label_name: &str, parent_layout: &VBoxLayout) -> LineEdit {
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

    ///
    /// Create a label + multi line text
    ///
    fn create_label_plain_text(rute: PluginUi, label_name: &str, parent_layout: &VBoxLayout) -> PlainTextEdit {
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
}

#[no_mangle]
pub fn hippo_view_plugin() -> *const std::os::raw::c_void {
    define_view_plugin!(PLUGIN, b"Song Info\0", b"0.0.1\0", SongInfo);
    // Would be nice to get rid of this
    let ret: *const std::os::raw::c_void = unsafe { std::mem::transmute(&PLUGIN) };
    ret
}


