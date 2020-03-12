extern crate rodio;

use crate::plugin_handler::DecoderPlugin;
use rodio::{Sink, Source};
use std::ffi::CString;
use std::os::raw::c_void;
use std::time::Duration;

use crate::service_ffi::PluginService;
use ringbuf::{Consumer, Producer, RingBuffer};

pub struct HippoPlayback {
    plugin_user_data: u64,
    plugin: DecoderPlugin,
    _read_stream: Consumer<Box<[u8]>>,
    out_data: Vec<f32>,
    frame_size: usize,
    current_offset: usize, // sender: Sender<DecodeEvent>,
}

pub struct Instance {
    plugin_user_data: u64,
    plugin: DecoderPlugin,
    pub write_stream: Producer<Box<[u8]>>,
}

#[derive(Default)]
pub struct MusicInfo {
    pub title: String,
    pub duration: i32,
}

impl HippoPlayback {
    pub fn start_with_file(
        plugin: &DecoderPlugin,
        plugin_service: &PluginService,
        filename: &str,
    ) -> Option<(HippoPlayback, Instance)> {
        let c_filename = CString::new(filename).unwrap();
        let user_data =
            unsafe { ((plugin.plugin_funcs).create)(plugin_service.get_c_service_api()) } as u64;
        let ptr_user_data = user_data as *mut c_void;
        let frame_size = 48000;
        //let frame_size = (((plugin.plugin_funcs).frame_size)(ptr_user_data)) as usize;
        // TODO: Verify that state is ok
        let _open_state =
            unsafe { ((plugin.plugin_funcs).open)(ptr_user_data, c_filename.as_ptr()) };

        let rb = RingBuffer::<Box<[u8]>>::new(256);
        let (prod, cons) = rb.split();

        Some((
            HippoPlayback {
                plugin_user_data: user_data,
                plugin: plugin.clone(),
                out_data: vec![0.0; frame_size],
                _read_stream: cons,
                frame_size,
                current_offset: frame_size + 1,
            },
            Instance {
                write_stream: prod,
                plugin_user_data: user_data,
                plugin: plugin.clone(),
            },
        ))
    }
}

impl Iterator for HippoPlayback {
    type Item = f32;

    #[inline]
    fn next(&mut self) -> Option<f32> {
        self.current_offset += 1;
        /*
        let message_count = self.incoming_messages.read_stream.len();

        for _ in message_count {
            let message = self.incoming_messages.read_stream.pop().unwrap();

            if let Some(event_callback) = (self.plugin.plugin_funcs).event {
                event_callback(
                    self.plugin_user_data as *mut c_void,


            }
        }
        */

        if self.current_offset >= self.frame_size {
            self.frame_size = unsafe {
                ((self.plugin.plugin_funcs).read_data)(
                    self.plugin_user_data as *mut c_void,
                    self.out_data.as_slice().as_ptr() as *mut c_void,
                    48000 / 2,
                ) as usize
            };

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
    pub playbacks: Vec<Instance>, // audio_endpoint: rodio::Endpoint,
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
            unsafe { ((pb.plugin.plugin_funcs).close)(pb.plugin_user_data as *mut c_void) };
        }
    }

    //pub fn pause(&mut self) {
    //    self.audio_sink.pause();
    //}

    //pub fn play(&mut self) {
    //   self.audio_sink.play();
    //}

    pub fn start_with_file(
        &mut self,
        plugin: &DecoderPlugin,
        service: &PluginService,
        filename: &str,
    ) -> MusicInfo {
        // TODO: Do error checking
        let playback = HippoPlayback::start_with_file(plugin, service, filename);

        if let Some(pb) = playback {
            // TODO: Wrap this
            let title = "Fixme".to_owned();

            //let c_title = ((plugin.plugin_funcs).track_info)(pb.plugin_user_data as *mut c_void);
            //let length = ((plugin.plugin_funcs).length)(pb.plugin_user_data as *mut c_void);
            let length = -10;
            let info = MusicInfo {
                //title: CStr::from_ptr(c_title).to_string_lossy().into_owned(),
                title,
                duration: length,
            };

            self.audio_sink.append(pb.0);
            self.playbacks.push(pb.1);

            info
        } else {
            MusicInfo::default()
        }
    }
}
