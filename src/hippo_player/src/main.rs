extern crate hippo_api;
extern crate messages;
extern crate rodio;
extern crate walkdir;

#[macro_use]
extern crate serde_derive;
pub extern crate rmp;
extern crate rmp_rpc;
extern crate serde;
extern crate serde_json;

extern crate rmp_serde as rmps;

#[macro_use]
extern crate rute;

extern crate dynamic_reload;

mod audio;
mod playlist;
mod plugin_handler;
mod song_db;

mod service;
pub mod service_ffi;

pub use rmp as msgpack;

use audio::{HippoAudio, MusicInfo};
use hippo_api::ffi::CMessageDecode;
use hippo_api::{MessageDecode, MessageEncode};
use playlist::Playlist;
use plugin_handler::Plugins;
use std::env;
use std::fs;
use std::fs::File;
use std::io;
use std::io::prelude::*;
use std::path::Path;

use rute::rute::*;
use rute::{SharedLibUi, Ui};
//use playerview::PlayerView;
//use playlist_view::PlaylistView;
//use song_info::SongInfoView;

#[derive(Default, Serialize, Deserialize)]
struct ViewInstanceState {
    plugin_name: String,
    instance_id: String,
    #[serde(skip)]
    instance: Option<plugin_handler::ViewPluginInstance>,
    #[serde(skip)]
    plugin_service: Option<service_ffi::PluginService>,
}

#[derive(Default, Serialize, Deserialize)]
struct HippoState {
    view_instance_states: Vec<ViewInstanceState>,
    layout: String,
}

struct HippoPlayer<'a> {
    audio: HippoAudio,
    plugins: Plugins<'a>,
    plugin_service: service_ffi::PluginService,
    main_widget: Widget,
    playlist: Playlist,
    dock_manager: DockManager,
    ui: Ui,
    app: Application,
    state: HippoState,
    _current_song_time: f32,
    _is_playing: bool,
}

include!(concat!(env!("OUT_DIR"), "/build_id.rs"));

impl<'a> HippoPlayer<'a> {
    pub fn new(ui: Ui) -> HippoPlayer<'a> {
        HippoPlayer {
            audio: HippoAudio::new(),
            plugins: Plugins::new(),
            plugin_service: service_ffi::PluginService::new(),
            app: ui.create_application(),
            main_widget: ui.create_widget(),
            playlist: Playlist::new(),
            dock_manager: ui.create_dock_manager(),
            state: HippoState::default(),
            ui: ui,
            _current_song_time: -10.0,
            _is_playing: false,
        }
    }

    ///
    /// Loops over all view instances and checks if any of them has posted any messages
    ///
    /// Currently we only pass all data to the playlist and then back to to the plugins
    ///
    fn update_ui_messages(&mut self) {
        let mut playlist_respones = Vec::new();
        let mut start_next_song = false;

        // Send all the events to the playlist

        for instance in &self.state.view_instance_states {
            if let Some(ref service) = instance.plugin_service {
                let messages = service.get_message_api();

                for message in &messages.request_queue {
                    let mut message_dec = MessageDecode::new(&message.data).unwrap();

                    //
                    // Parse messages that affects both player and playlist
                    //

                    match message_dec.method {
                        "hippo_playlist_next_song" => {
                            start_next_song = true;
                        }

                        _ => (),
                    }

                    self.playlist
                        .event(&mut message_dec)
                        .map(|reply| playlist_respones.push(reply));
                }
            }
        }

        if start_next_song {
            self.playlist
                .get_next_song()
                .map(|song| self.play_file(&song));
        }

        for instance in &mut self.state.view_instance_states {
            if let Some(ref mut service) = instance.plugin_service {
                let messages = service.get_message_api_mut();
                messages.clear_queues();
            }
        }

        // Send back the replies from the playlist

		self.send_messages_to_plugins(&playlist_respones);
    }

    fn send_message_to_plugins(&mut self, msg: &MessageEncode) {
		let message_dec = MessageDecode::new(&msg.data).unwrap();
		let message = service_ffi::get_cmessage_decode(&message_dec);

		for instance in &mut self.state.view_instance_states {
			let pb = instance.instance.as_ref().unwrap();
			let ptr: *const CMessageDecode = &message as *const CMessageDecode;
			((pb.plugin.plugin_funcs).event.unwrap())(
				pb.user_data as *mut ::std::os::raw::c_void,
				ptr,
			);
		}
    }

    fn send_messages_to_plugins(&mut self, messages: &Vec<MessageEncode>) {
        for msg in messages {
        	self.send_message_to_plugins(&msg);
        }
    }

    /*
    fn stop_song(&mut self) {
        self.audio.stop();
        self.is_playing = false;
    }
    */

    fn try_save_layout(filename: &str, layout: &str) -> io::Result<()> {
        let mut file = File::create(filename)?;
        file.write_all(layout.as_bytes())?;
        Ok(())
    }

    fn before_quit(&mut self) {
        self.state.layout = self.dock_manager.save_state();

        let state = serde_json::to_string_pretty(&self.state).unwrap();

        println!("state {}", state);

        // TODO: Error handling
        Self::try_save_layout("layout.data", &state).unwrap();

		if let Err(err) = self.playlist.save("playlist.json") {
			println!("Unable to save playlist {:?}", err);
		}
	}

	fn load_playlist(&mut self, filename: &str) {
		let res = match self.playlist.load(filename) {
			Err(err) => return,
			Ok(t) => t,
		};

		self.send_message_to_plugins(&res);
	}

    fn add_files(&mut self) {
    	/*
		for url in self.app.get_files().iter().filter(|u| u.is_local_file()) {
		self.playlist.add_file(&url.to_local_file());
		}
		*/
    }

    fn show_plugin(&mut self, action: &Action) {
        let plugin_index = action.get_int_data() as usize;
        let widget = self.ui.create_widget();

        let instance;
        let plugin_service = service_ffi::PluginService::new();

        {
            let plugin = &self.plugins.view_plugins[plugin_index];
            instance = plugin.create_instance(&self.ui, &plugin_service, &widget);
        }

        let dock_widget = self.ui.create_dock_widget();

        self.plugins.view_plugins[plugin_index].count += 1;

        // Add instance count to name if > 0

        let object_name = match instance.id {
            0 => action.text().to_owned(),
            e @ _ => format!("{} {}", action.text(), e),
        };

        dock_widget.set_object_name(&object_name);
        dock_widget.set_widget(&widget);

        self.dock_manager.add_to_docking(&dock_widget);

        widget.resize(500, 500);
        widget.show();

        widget.set_persist_data("pls save me pls!");

        let view_state = ViewInstanceState {
            plugin_name: action.text().to_owned(),
            instance_id: object_name.to_owned(),
            instance: Some(instance),
            plugin_service: Some(plugin_service),
        };

        self.state.view_instance_states.push(view_state);

        println!("Showing plugin {}", plugin_index);
    }

    fn create_plugins_menu(&mut self) -> Menu {
        let plugin_menu = self.ui.create_menu();
        plugin_menu.set_title("Views");

        set_menu_triggered_event!(plugin_menu, self, HippoPlayer, HippoPlayer::show_plugin);

        for (i, plugin) in self.plugins.view_plugins.iter().enumerate() {
            let name = plugin.get_name();

            let action = self.ui.create_action();
            action.set_text(&name);
            action.set_int_data(i as i32);

            plugin_menu.add_action(&action);
        }

        plugin_menu
    }

    ///
    /// This will load a defult layout file and set up plugins for it
    ///
    fn load_layout(&mut self, filename: &str) {
        let mut state = String::new();
        let mut file = match File::open(filename) {
            Ok(file) => file,
            Err(_) => {
                println!("Unable to open layout {}", filename);
                return;
            }
        };

        file.read_to_string(&mut state).unwrap();

        // Parse the string of data into serde_json::Value.
        let mut state: HippoState = serde_json::from_str(&state).unwrap();

        // create the plugins and add them to the docking manager, then then load the layout

        for view_state in &mut state.view_instance_states {
            for plugin in &self.plugins.view_plugins {
                if plugin.get_name() != view_state.plugin_name {
                    continue;
                }

                let dock_widget = self.ui.create_dock_widget();
                let widget = self.ui.create_widget();

                let plugin_service = service_ffi::PluginService::new();
                let instance = plugin.create_instance(&self.ui, &plugin_service, &widget);

                view_state.instance = Some(instance);
                view_state.plugin_service = Some(plugin_service);

                dock_widget.set_object_name(&view_state.instance_id);
                dock_widget.set_widget(&widget);

                self.dock_manager.add_to_docking(&dock_widget);

                widget.set_persist_data("pls save me pls!");
            }
        }

        self.dock_manager.restore_state(&state.layout);
        self.state = state;
    }

    pub fn run(&mut self) {
        self.app.set_style_sheet("bin/player/themes/dark/style.qss");

        let main_window = self.ui.create_main_window();
        let timer = self.ui.create_timer();

        let layout = self.ui.create_v_box_layout();

        let add_files = self.ui.create_action();
        add_files.set_shortcut_mod(Keys::KeyO, MetaKeys::Ctrl);
        add_files.set_text("Add Files");

        let file_menu = self.ui.create_menu();
        file_menu.set_title("File");
        file_menu.add_action(&add_files);

        let plugin_menu = self.create_plugins_menu();

        let menu_bar = main_window.menu_bar();
        menu_bar.add_menu(&file_menu);
        menu_bar.add_menu(&plugin_menu);

        set_timer_timeout_event!(timer, self, HippoPlayer, HippoPlayer::update_ui_messages);
        set_action_triggered_event!(add_files, self, HippoPlayer, HippoPlayer::add_files);
        set_application_about_to_quit_event!(self.app, self, HippoPlayer, HippoPlayer::before_quit);

        timer.start(5);

        self.main_widget.set_layout(&layout);
        self.main_widget.resize(1200, 1000);

        main_window.set_central_widget(&self.main_widget);

        self.dock_manager.set_parent(&self.main_widget);

        layout.add_widget(&self.dock_manager);

        //player_window.set_content(&main_window);
        //player_window.set_window_title(&format!("HippoPlayer 0.0.1 {}", build_id()));

        main_window.resize(800, 600);

        self.load_layout("layout.data");
        self.load_playlist("playlist.json");

        main_window.show();

        self.app.exec();
    }

    pub fn play_file(&mut self, filename: &str) -> MusicInfo {
        let mut buffer = [0; 4096];
        // TODO: Fix error handling here

        let metadata = fs::metadata(filename).unwrap();
        let mut f = File::open(filename).unwrap();
        let buffer_read_size = f.read(&mut buffer[..]).unwrap();

        // TODO: Proper priority order of plugins. Right now we just put uade as the last one to
        // check

        // find a plugin that supports the file

        for plugin in &self.plugins.decoder_plugins {
            if plugin.plugin_path.rfind("uade").is_some() {
                continue;
            }

            if plugin.probe_can_play(&buffer, buffer_read_size, filename, metadata.len()) {
                // This is a bit hacky right now but will do the trick
                self.audio.stop();
                self.audio = HippoAudio::new();
                let info = self
                    .audio
                    .start_with_file(&plugin, &self.plugin_service, filename);

                //self.song_info_view.update_data(filename, &self.plugin_service.get_song_db());
                return info;
            }
        }

        // do the same fo uade

        for plugin in &self.plugins.decoder_plugins {
            if plugin.plugin_path.rfind("uade").is_none() {
                continue;
            }

            if plugin.probe_can_play(&buffer, buffer_read_size, filename, metadata.len()) {
                // This is a bit hacky right now but will do the trick
                self.audio.stop();
                self.audio = HippoAudio::new();
                let info = self
                    .audio
                    .start_with_file(&plugin, &self.plugin_service, filename);
                return info;
            }
        }

        println!("Unable to find plugin to support {}", filename);

        MusicInfo::default()
    }
}

impl<'a> Drop for HippoPlayer<'a> {
    fn drop(&mut self) {
        self.audio.stop();
    }
}

fn main() {
    let _args: Vec<String> = env::args().collect();

    let current_path = std::env::current_dir().unwrap();

    let rute_instance = SharedLibUi::new().unwrap();
    let ui = rute_instance.get_ui();

    let mut app = HippoPlayer::new(ui);

    unsafe {
        let service_api = app.plugin_service.get_c_service_api();
        println!("{:?}", service_api);
        ((*service_api).get_io_api)((*service_api).priv_data, 1);
    }

    app.plugins.add_plugins_from_path();

    // No need to switch back if we are in the correct spot

    if !Path::new("bin").is_dir() {
        std::env::set_current_dir(current_path).unwrap();
    }

    app.run();

    // we exit directly here because of Qt and the way its static destructors works
    // causes lots of issues because we have the Qt code in a separate dll
    std::process::exit(0);
}
