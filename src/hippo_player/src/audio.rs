extern crate rodio;

use plugin_handler::DecoderPlugin;
use rodio::{Source, Sink};
use std::ffi::CString;
use std::os::raw::c_void;
use std::time::Duration;

// #[derive(Clone)]
// pub enum DecodeEvent {
//    Position(usize),
//    Data(Vec<u8>),
// }

#[derive(Clone)]
pub struct HippoPlayback {
    plugin_user_data: u64,
    plugin: DecoderPlugin,
    // temp_data: Vec<i16>,
    out_data: Vec<f32>,
    frame_size: usize,
    current_offset: usize, // sender: Sender<DecodeEvent>,
}

impl HippoPlayback {
    pub fn start_with_file(plugin: &DecoderPlugin, filename: &str) -> HippoPlayback {
        let c_filename = CString::new(filename).unwrap();
        let user_data = ((plugin.plugin_funcs).create)() as u64;
        let ptr_user_data = user_data as *mut c_void;
        let frame_size = (((plugin.plugin_funcs).frame_size)(ptr_user_data)) as usize;
        // let _open_state = ((plugin.plugin_funcs).open)(ptr_user_data, b"bin/player/songs/ahx/geir_tjelta_-_a_new_beginning.ahx\0".as_ptr());
        // let _open_state = ((plugin.plugin_funcs).open)(ptr_user_data, b"bin/player/songs/mod/global_trash_3_v2.mod\0".as_ptr());
        // TODO: Verify that state is ok
        let _open_state = ((plugin.plugin_funcs).open)(ptr_user_data, c_filename.as_ptr());

        HippoPlayback {
            plugin_user_data: user_data,
            plugin: plugin.clone(),
            // temp_data: vec![0; frame_size],
            out_data: vec![0.0; frame_size],
            frame_size: frame_size,
            current_offset: frame_size + 1, // sender: sender,
        }
    }
}

impl Iterator for HippoPlayback {
    type Item = f32;

    #[inline]
    fn next(&mut self) -> Option<f32> {
        self.current_offset += 1;

        if self.current_offset >= self.frame_size {
            ((self.plugin.plugin_funcs).read_data)(self.plugin_user_data as *mut c_void,
                                                   self.out_data.as_slice().as_ptr() as *mut u8);

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
    fn samples_rate(&self) -> u32 {
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
        let endpoint = rodio::get_default_endpoint().unwrap();
        let sink = Sink::new(&endpoint);

        HippoAudio {
            audio_sink: sink,
            // audio_endpoint: endpoint,
            playbacks: Vec::new(),
        }
    }

    pub fn start_with_file(&mut self, plugin: &DecoderPlugin, filename: &str) {
        // TODO: Do error checking
        let playback = HippoPlayback::start_with_file(plugin, filename);
        self.playbacks.push(playback.clone());
        self.audio_sink.append(playback);
    }
}
