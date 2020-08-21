//extern crate rodio;

use miniaudio::{Devices};
use messages::*;

use crate::plugin_handler::DecoderPlugin;
//use rodio::{Sink, Source};
use std::ffi::CString;
use std::os::raw::c_void;
use std::time::Duration;
use std::sync::{Arc, Mutex};

//use miniaudio::{Device, DeviceConfig, DeviceType, Format, FramesMut};
//use miniaudio::{Waveform, WaveformConfig, WaveformType};

use crate::service_ffi::PluginService;
use ringbuf::{Consumer, Producer, RingBuffer};

#[derive(Clone)]
pub struct HippoPlayback {
    plugin_user_data: u64,
    plugin: DecoderPlugin,
}

pub struct Instance {
    plugin_user_data: u64,
    plugin: DecoderPlugin,
    pub write_stream: Producer<Box<[u8]>>,
}

impl HippoPlayback {
    pub fn start_with_file(
        plugin: &DecoderPlugin,
        plugin_service: &PluginService,
        filename: &str,
    ) -> Option<(HippoPlayback, Instance)> {
        let c_filename;
        let subsong_index;
        // Find subsong separator
        // TODO: store subsong index instead?
        if let Some(separator) = filename.find('|') {
            // create filename without separator
            c_filename = CString::new(&filename[..separator]).unwrap();
            subsong_index = *&filename[separator + 1..].parse::<i32>().unwrap();
        } else {
            c_filename = CString::new(filename).unwrap();
            subsong_index = 0i32;
        }

        let user_data =
            unsafe { ((plugin.plugin_funcs).create)(plugin_service.get_c_service_api()) } as u64;
        let ptr_user_data = user_data as *mut c_void;
        let frame_size = 48000;
        //let frame_size = (((plugin.plugin_funcs).frame_size)(ptr_user_data)) as usize;
        // TODO: Verify that state is ok
        let _open_state =
            unsafe { ((plugin.plugin_funcs).open)(ptr_user_data, c_filename.as_ptr(), subsong_index) };

        /*
        if open_state < 0 {
            return None;
        }
        */

        let rb = RingBuffer::<Box<[u8]>>::new(256);
        let (prod, cons) = rb.split();

        Some((
            HippoPlayback {
                plugin_user_data: user_data,
                plugin: plugin.clone(),
                //_read_stream: cons,
            },
            Instance {
                write_stream: prod,
                plugin_user_data: user_data,
                plugin: plugin.clone(),
            },
        ))
    }
}
/*
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
*/

/*
struct AudioPlayback {
    value: f32,
}

impl miniaudio::Source for AudioPlayback {
    fn generate_samples(&mut self, output: &mut [f32]) {
        let mut start = self.value;

        println!("gen samples {}", output.len());

        for v in output {
            let t = start.sin();
            //println!("{}", t);
            *v = t;
            start += 0.0001;
        }

        self.value += 0.001;
    }
}
*/

pub struct HippoAudio {
    //players: Box<Mutex<Vec<HippoPlayback>>>,
    players: *mut c_void,
    output_devices: Option<Devices>,
    pub playbacks: Vec<Instance>,
}

unsafe extern "C" fn data_callback(
    device_ptr: *mut miniaudio::ma_device,
    output_ptr: *mut c_void,
    _input_ptr: *const c_void,
    frame_count: u32,
) {
    let source: &Mutex<Vec<HippoPlayback>> = std::mem::transmute((*device_ptr).pUserData);
    let playback;

    {
        // miniaudio will clear the buffer so we don't have to do it here
        let t = source.lock().unwrap();
        if t.len() == 0 {
            return;
        }

        playback = t[0].clone();
    }

    ((playback.plugin.plugin_funcs).read_data)(
        playback.plugin_user_data as *mut c_void,
        output_ptr, frame_count);
}



impl HippoAudio {
    pub fn new() -> HippoAudio {
        // This is a bit hacky so it can be shared with the device and HippoAudio
        let players = Box::new(Mutex::new(Vec::<HippoPlayback>::new()));

        HippoAudio {
            players: Box::into_raw(players) as *mut c_void,
            output_devices: None,
            playbacks: Vec::new(),
        }
    }

    pub fn stop(&mut self) {
        /*
        self.audio_sink.stop();
        self.audio_sink.sleep_until_end();

        for pb in self.playbacks.iter_mut() {
            unsafe { ((pb.plugin.plugin_funcs).close)(pb.plugin_user_data as *mut c_void) };
        }

        self.playbacks.clear();
        */
    }

    fn replay_output_devices(&self) -> Option<Box<[u8]>> {
        let output_devices = self.output_devices.as_ref()?;

        let mut builder = messages::FlatBufferBuilder::new_with_capacity(8192);
        let mut out_ent = Vec::with_capacity(output_devices.devices.len());

        for dev in &output_devices.devices {
            let device_name = builder.create_string(&dev.name);

            let desc = HippoOutputDevice::create(
                &mut builder,
                &HippoOutputDeviceArgs {
                    name: Some(device_name),
                    min_channels: dev.min_channels as i32,
                    max_channels: dev.max_channels as i32,
                    min_sample_rate: dev.min_sample_rate as i32,
                    max_sample_rate: dev.max_channels as i32,
                },
            );

            out_ent.push(desc);
        }

        let devices_vec = builder.create_vector(&out_ent);

        let added_devices = HippoReplyOutputDevices::create(
            &mut builder,
            &HippoReplyOutputDevicesArgs {
                devices: Some(devices_vec),
            },
        );

        Some(HippoMessage::create_def(
            builder,
            MessageType::reply_output_devices,
            added_devices.as_union_value(),
        ))
    }

    ///
    /// Handle incoming events
    ///
    pub fn event(&mut self, msg: &HippoMessage) -> Option<Box<[u8]>> {
        match msg.message_type() {
            MessageType::request_output_devices => self.replay_output_devices(),
            _ => None,
        }
    }

    pub fn init_output_device(&mut self) -> Result<(), miniaudio::Error> {
        self.output_devices = Some(Devices::new()?);


        Ok(())
    }

    pub fn init_devices(&mut self) -> Result<(), miniaudio::Error> {
        Ok(())
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
    ) -> Result<(), miniaudio::Error> {


        /*
        // TODO: Do error checking
        let playback = HippoPlayback::start_with_file(plugin, service, filename);

        if let Some(pb) = playback {
            {
                let players: &Mutex<Vec<HippoPlayback>> = unsafe { std::mem::transmute(self.players) };
                let mut t = players.lock().unwrap();
                t.push(pb.0);
            }
            self.playbacks.push(pb.1);
        }
        */

        Ok(())
    }
}
