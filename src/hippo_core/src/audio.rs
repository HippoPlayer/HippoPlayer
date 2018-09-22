extern crate rodio;

use decoder_plugins::DecoderPlugin;
use rodio::{Source, Sink};
use service_ffi::PluginService;
use std::ffi::CString;
use std::os::raw::c_void;
use std::time::Duration;

#[derive(Clone)]
pub struct HippoPlayback {
    plugin_user_data: u64,
    plugin: DecoderPlugin,
    out_data: Vec<f32>,
    frame_size: usize,
    current_offset: usize, // sender: Sender<DecodeEvent>,
}

#[derive(Default)]
pub struct MusicInfo {
    pub title: String,
    pub duration: i32,
}

impl HippoPlayback {
    pub fn start_with_file(plugin: &DecoderPlugin, plugin_service: &PluginService, filename: &str) -> Option<HippoPlayback> {
        let c_filename = CString::new(filename).unwrap();
        let user_data = ((plugin.plugin_funcs).create)(plugin_service.get_c_service_api()) as u64;
        let ptr_user_data = user_data as *mut c_void;
        let frame_size = 48000;
        //let frame_size = (((plugin.plugin_funcs).frame_size)(ptr_user_data)) as usize;
        // TODO: Verify that state is ok
        let _open_state = ((plugin.plugin_funcs).open)(ptr_user_data, c_filename.as_ptr());

        Some(HippoPlayback {
            plugin_user_data: user_data,
            plugin: plugin.clone(),
            out_data: vec![0.0; frame_size],
            frame_size: frame_size,
            current_offset: frame_size + 1, // sender: sender,
        })
    }
}

impl Iterator for HippoPlayback {
    type Item = f32;

    #[inline]
    fn next(&mut self) -> Option<f32> {
        self.current_offset += 1;

        if self.current_offset >= self.frame_size {
            self.frame_size = ((self.plugin.plugin_funcs)
                                   .read_data)(self.plugin_user_data as *mut c_void,
                                               self.out_data.as_slice().as_ptr() as *mut u8, 48000/2) as usize;

            self.current_offset = 0;
        }

        Some(self.out_data[self.current_offset])
    }
}

impl Source for HippoPlayback {
    #[inline]
    fn current_frame_len(&self) -> Option<usize> {
        None
    }

    #[inline]
    fn channels(&self) -> u16 {
        2
    }

    #[inline]
    fn sample_rate(&self) -> u32 {
        48000
    }

    #[inline]
    fn total_duration(&self) -> Option<Duration> {
        None
    }
}

pub struct HippoAudio {
    audio_sink: rodio::Sink,
    playbacks: Vec<HippoPlayback>, // audio_endpoint: rodio::Endpoint,
}

impl HippoAudio {
    pub fn new() -> HippoAudio {
        let device = rodio::default_output_device().unwrap();
        let sink = Sink::new(&device);

        HippoAudio {
            audio_sink: sink,
            playbacks: Vec::new(),
        }
    }

    pub fn stop(&mut self) {
        self.audio_sink.stop();
        self.audio_sink.sleep_until_end();

        for pb in self.playbacks.iter_mut() {
            ((pb.plugin.plugin_funcs).close)(pb.plugin_user_data as *mut c_void);
        }
    }

    //pub fn pause(&mut self) {
    //    self.audio_sink.pause();
    //}

    //pub fn play(&mut self) {
    //   self.audio_sink.play();
    //}

    pub fn start_with_file(&mut self, plugin: &DecoderPlugin, service: &PluginService, filename: &str) -> MusicInfo {
        // TODO: Do error checking
        let playback = HippoPlayback::start_with_file(plugin, service, filename);

        if let Some(pb) = playback {
            // TODO: Wrap this
			//let title = service.get_song_db().get_key(filename, 0, "title").unwrap();
			let title = service.get_song_db().get_key(filename, 0, "title").or_else(|| Some("Unknown".to_owned()));

			//let c_title = ((plugin.plugin_funcs).track_info)(pb.plugin_user_data as *mut c_void);
			//let length = ((plugin.plugin_funcs).length)(pb.plugin_user_data as *mut c_void);
			let length = -10;
			let info = MusicInfo {
				//title: CStr::from_ptr(c_title).to_string_lossy().into_owned(),
				title: title.unwrap(),
				duration: length,
			};

			self.playbacks.push(pb.clone());
			self.audio_sink.append(pb);

			info
        } else {
            MusicInfo::default()
        }
    }
}
