pub mod error;
pub mod sys;
use logger::*;

pub use error::Error;
use std::mem::MaybeUninit;
use std::os::raw::c_char;
use std::os::raw::c_void;
pub use sys::ma_device;

macro_rules! impl_from_c {
    ($RustType:ty, $CType:ty) => {
        impl $RustType {
            pub fn from_c(c_enum: $CType) -> $RustType {
                unsafe { std::mem::transmute(c_enum) }
            }
        }
    };
}

pub type DataCallback = unsafe extern "C" fn(
    device_ptr: *mut sys::ma_device,
    output_ptr: *mut c_void,
    _input_ptr: *const c_void,
    frame_count: u32,
);

/// This macro will execute a success block if a result is a MA_SUCCESS block
/// and return the value of that block wrapped in a Result::Ok. If $Result is an error this will
/// return an Error enum wrapped in a Result::Err.
macro_rules! map_result {
    ($Result:expr, $Success:expr) => {
        if $crate::Error::is_c_error($Result) {
            Err($crate::Error::from_c_error($Result))
        } else {
            Ok($Success)
        }
    };
}

/// Converts slice of any sized type into a slice of bytes.
pub fn into_byte_slice<T: Sized>(orig: &[T]) -> &[u8] {
    let byte_len = orig.len() * std::mem::size_of::<T>();
    let ptr = orig.as_ptr() as *const u8;
    let mut found_first_zero = 0;
    let byte_array = unsafe { std::slice::from_raw_parts(ptr, byte_len) };

    for t in byte_array {
        if *t == 0 {
            break;
        }

        found_first_zero += 1;
    }

    unsafe { std::slice::from_raw_parts(ptr, found_first_zero + 1) }
}

pub fn cstr_display<'s>(full_arr: &'s [c_char]) -> std::borrow::Cow<'s, str> {
    let byte_slice = into_byte_slice(full_arr);

    let t = unsafe { std::ffi::CStr::from_bytes_with_nul_unchecked(byte_slice) };
    t.to_string_lossy()
}

static LEVEL_TRANSLATION: &[i32] = &[
    0,
    HIPPO_LOG_ERROR,
    HIPPO_LOG_WARN,
    HIPPO_LOG_INFO,
    HIPPO_LOG_DEBUG,
];

unsafe extern "C" fn log_callback(
    _context: *mut sys::ma_context,
    _device: *mut sys::ma_device,
    level: sys::ma_uint32,
    message: *const ::std::os::raw::c_char,
) {
    // we have some more levels in our logger so bump up them to that range
    let level = level as usize;
    logger::hippo_log(
        std::ptr::null_mut(),
        LEVEL_TRANSLATION[level],
        std::ptr::null(),
        0,
        message,
    );
}

pub struct Device(*mut sys::ma_device);

pub struct OutputDevice {
    pub device_id: sys::ma_device_id,
    pub name: String,
    pub min_channels: usize,
    pub max_channels: usize,
    pub min_sample_rate: usize,
    pub max_sample_rate: usize,
}

pub struct Devices {
    pub context: *mut sys::ma_context,
    pub devices: Vec<OutputDevice>,
}

//
impl Devices {
    pub fn new() -> Result<Devices, Error> {
        let mut config = unsafe { sys::ma_context_config_init() };
        config.logCallback = Some(log_callback);
        config.pUserData = std::ptr::null_mut();

        let mut context = MaybeUninit::<sys::ma_context>::uninit();

        let result =
            unsafe { sys::ma_context_init(std::ptr::null_mut(), 0, &config, context.as_mut_ptr()) };

        if Error::is_c_error(result) {
            return Err(Error::from_c_error(result));
        }

        let context = unsafe { context.assume_init() };

        let context = Box::into_raw(Box::new(context));

        let mut playback_device_infos = MaybeUninit::<*mut sys::ma_device_info>::uninit();
        let mut playback_device_count = 0u32;

        let mut capture_device_infos = MaybeUninit::<*mut sys::ma_device_info>::uninit();
        let mut capture_device_count = 032;

        let result = unsafe {
            sys::ma_context_get_devices(
                context,
                playback_device_infos.as_mut_ptr(),
                &mut playback_device_count,
                capture_device_infos.as_mut_ptr(),
                &mut capture_device_count,
            )
        };

        if Error::is_c_error(result) {
            return Err(Error::from_c_error(result));
        }

        let playback_device_infos = unsafe { playback_device_infos.assume_init() };

        let mut devices = Vec::with_capacity(playback_device_count as usize);

        for device_idx in 0..playback_device_count {
            let device = unsafe { playback_device_infos.add(device_idx as usize) };
            let name = unsafe { cstr_display(&(*device).name) };
            let name = name.to_owned().to_string();

            trace!("Found output device {}", name);

            unsafe {
                devices.push(OutputDevice {
                    device_id: (*device).id,
                    name,
                    min_channels: (*device).minChannels as usize,
                    max_channels: (*device).maxChannels as usize,
                    min_sample_rate: (*device).minSampleRate as usize,
                    max_sample_rate: (*device).maxSampleRate as usize,
                });
            }
        }

        Ok(Devices { context, devices })
    }
}

unsafe extern "C" fn stop_callback(_device_ptr: *mut sys::ma_device) {}

impl Device {
    pub fn new(
        callback: DataCallback,
        user_data: *mut c_void,
        context: *mut sys::ma_context,
        device_id: Option<*const sys::ma_device_id>,
    ) -> Result<Device, Error> {
        let device = Box::into_raw(Box::new(MaybeUninit::<sys::ma_device>::uninit()));

        let mut device_config = unsafe { sys::ma_device_config_init(sys::ma_device_type_playback) };
        let dev = device_id.unwrap_or_else(|| std::ptr::null_mut());

        device_config.playback.pDeviceID = dev;
        device_config.playback.format = 0;
        device_config.playback.channels = 2;
        device_config.sampleRate = 0;
        device_config.dataCallback = Some(callback);
        device_config.stopCallback = Some(stop_callback);
        device_config.pUserData = user_data;

        // We don't need low-latency audio, larger buffers is better for us
        device_config.performanceProfile = sys::ma_performance_profile_conservative;

        let result =
            unsafe { sys::ma_device_init(context, &device_config, (*device).as_mut_ptr()) };

        map_result!(result, Device(unsafe { (*device).as_mut_ptr() }))
    }

    pub fn start(&self) -> Result<(), Error> {
        unsafe { Error::from_c_result(sys::ma_device_start(self.0 as *const _ as *mut _)) }
    }

    pub fn close(&self) {
        unsafe { sys::ma_device_uninit(self.0 as *const _ as *mut _) };
    }

    pub fn sample_rate(&self) -> u32 {
        unsafe { (*self.0).sampleRate as u32 }
    }

    pub fn channels(&self) -> u32 {
        unsafe { (*self.0).playback.channels as u32 }
    }

    pub fn format(&self) -> Format {
        unsafe { Format::from_c((*self.0).playback.format) }
    }
}

#[derive(Clone, Copy, PartialEq)]
pub enum ResampleAlgorithm {
    Linear {
        lpf_order: u32,
        lpf_nyquist_factor: f64,
    },

    Speex {
        quality: u32,
    },
}

#[repr(C)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Format {
    Unknown = sys::ma_format_unknown as _,
    U8 = sys::ma_format_u8 as _,
    S16 = sys::ma_format_s16 as _,
    S24 = sys::ma_format_s24 as _,
    S32 = sys::ma_format_s32 as _,
    F32 = sys::ma_format_f32 as _,
}

impl_from_c!(Format, sys::ma_format);

impl Format {
    /// The size of one sample in this format in bytes.
    pub fn size_in_bytes(self) -> usize {
        match self {
            Self::Unknown => 0,
            Self::U8 => 1,
            Self::S16 => 2,
            Self::S24 => 3,
            Self::S32 => 4,
            Self::F32 => 4,
        }
    }
}

#[repr(transparent)]
pub struct DataConverterConfig(sys::ma_data_converter_config);

impl DataConverterConfig {
    pub fn new(
        format_in: Format,
        format_out: Format,
        channels_in: u32,
        channels_out: u32,
        sample_rate_in: u32,
        sample_rate_out: u32,
        resampler: ResampleAlgorithm,
    ) -> DataConverterConfig {
        let mut cfg = DataConverterConfig(unsafe {
            sys::ma_data_converter_config_init(
                format_in as _,
                format_out as _,
                channels_in,
                channels_out,
                sample_rate_in,
                sample_rate_out,
            )
        });

		cfg.0.resampling.allowDynamicSampleRate = sys::MA_TRUE;
        cfg.set_resampling(resampler);
        cfg
    }

    #[inline]
    pub fn format_in(&self) -> Format {
        Format::from_c(self.0.formatIn)
    }

    #[inline]
    pub fn set_format_in(&mut self, format: Format) {
        self.0.formatIn = format as _;
    }
    #[inline]
    pub fn format_out(&self) -> Format {
        Format::from_c(self.0.formatOut)
    }

    #[inline]
    pub fn set_format_out(&mut self, format: Format) {
        self.0.formatOut = format as _;
    }

    #[inline]
    pub fn channels_in(&self) -> u32 {
        self.0.channelsIn
    }

    #[inline]
    pub fn set_channels_in(&mut self, channels: u32) {
        self.0.channelsIn = channels;
    }

    #[inline]
    pub fn channels_out(&self) -> u32 {
        self.0.channelsOut
    }

    #[inline]
    pub fn set_channels_out(&mut self, channels: u32) {
        self.0.channelsOut = channels;
    }

    pub fn sample_rate_in(&self) -> u32 {
        self.0.sampleRateIn
    }

    pub fn set_sample_rate_in(&mut self, rate: u32) {
        self.0.sampleRateIn = rate;
    }

    pub fn sample_rate_out(&self) -> u32 {
        self.0.sampleRateOut
    }

    pub fn set_resampling(&mut self, algo: ResampleAlgorithm) {
        match algo {
            ResampleAlgorithm::Linear {
                lpf_order,
                lpf_nyquist_factor,
            } => {
                self.0.resampling.algorithm = sys::ma_resample_algorithm_linear;
                self.0.resampling.linear.lpfOrder = lpf_order;
                self.0.resampling.linear.lpfNyquistFactor = lpf_nyquist_factor;
            }

            ResampleAlgorithm::Speex { quality } => {
                self.0.resampling.algorithm = sys::ma_resample_algorithm_speex;
                self.0.resampling.speex.quality = quality as _;
            }
        }
    }

    pub fn resampling(&self) -> ResampleAlgorithm {
        match self.0.resampling.algorithm {
            sys::ma_resample_algorithm_linear => ResampleAlgorithm::Linear {
                lpf_order: self.0.resampling.linear.lpfOrder,
                lpf_nyquist_factor: self.0.resampling.linear.lpfNyquistFactor,
            },

            sys::ma_resample_algorithm_speex => ResampleAlgorithm::Speex {
                quality: self.0.resampling.speex.quality as _,
            },

            _ => unreachable!(),
        }
    }
}


#[repr(transparent)]
pub struct DataConverter(sys::ma_data_converter);

impl DataConverter {
    pub fn new(config: &DataConverterConfig) -> Result<DataConverter, Error> {
        let mut converter = std::mem::MaybeUninit::<DataConverter>::uninit();
        unsafe {
            Error::from_c_result(sys::ma_data_converter_init(
                &config.0 as *const _,
                converter.as_mut_ptr().cast(),
            ))?;
            Ok(converter.assume_init())
        }
    }

    #[inline]
    pub fn process_pcm_frames(
        &mut self,
        output: *mut c_void,
        input: *const c_void,
        output_frame_count: usize,
        input_frame_count: usize,
    ) -> Result<(u64, u64), Error> {
        let mut input_frame_count = input_frame_count as u64;
        let mut output_frame_count = output_frame_count as u64;

        Error::from_c_result(unsafe {
            sys::ma_data_converter_process_pcm_frames(
                &mut self.0,
                input as *const _,
                &mut input_frame_count,
                output as *mut _,
                &mut output_frame_count,
            )
        })?;

        Ok((output_frame_count, input_frame_count))
    }

    pub fn set_rate(&mut self, sample_rate_in: u32, sample_rate_out: u32) -> Result<(), Error> {
        Error::from_c_result(unsafe {
            sys::ma_data_converter_set_rate(&mut self.0, sample_rate_in, sample_rate_out)
        })
    }

    pub fn set_rate_ratio(&mut self, ratio_in_out: f32) -> Result<(), Error> {
        Error::from_c_result(unsafe {
            sys::ma_data_converter_set_rate_ratio(&mut self.0, ratio_in_out)
        })
    }

    pub fn required_input_frame_count(&self, output_frame_count: u64) -> u64 {
        unsafe {
            sys::ma_data_converter_get_required_input_frame_count(
                &self as *const _ as *mut _,
                output_frame_count,
            )
        }
    }

    pub fn expected_output_frame_count(&mut self, input_frame_count: u64) -> u64 {
        unsafe {
            sys::ma_data_converter_get_expected_output_frame_count(
                &mut self.0,
                input_frame_count,
            )
        }
    }

    pub fn input_latency(&self) -> u64 {
        unsafe { sys::ma_data_converter_get_input_latency(&self as *const _ as *mut _) }
    }


    pub fn output_latency(&self) -> u64 {
        unsafe { sys::ma_data_converter_get_output_latency(&self as *const _ as *mut _) }
    }

    pub fn config(&self) -> &sys::ma_data_converter_config {
        &self.0.config
    }


    /// Re-constructs the resampler if any of the input parameters differs
    pub fn update(
        &mut self,
        input_channel_count: u8,
        input_format: Format,
        input_sample_rate: u32,
    ) {
        let channels = self.0.config.channelsIn == input_channel_count as u32;
        let format = Format::from_c(self.0.config.formatIn) == input_format;
        let sample_rate = self.0.config.sampleRateIn == input_sample_rate;

        // if we have the same input format we don't need to change anything
        if channels && format && sample_rate {
        	println!("Nothing to update");
            return;
        }

        // if only sample rate is different we can update it without recrating the object
        if !sample_rate && channels && format {
            self.set_rate(input_sample_rate, self.0.config.sampleRateOut)
                .unwrap();
            self.0.config.sampleRateIn = input_sample_rate;
            return;
        }

        let mut cfg = self.0.config;
        cfg.formatIn = input_format as _;
        cfg.channelsIn = input_channel_count as u32;
        cfg.sampleRateIn = input_sample_rate;
		//cfg.resampling.allowDynamicSampleRate = sys::MA_TRUE;

        //println!("output rate {}", cfg.sampleRateOut);

        unsafe {
            sys::ma_data_converter_uninit(&mut self.0);
            sys::ma_data_converter_init(&cfg as *const _, &mut self.0 as *mut _);
        }
    }
}






impl Drop for DataConverter {
    fn drop(&mut self) {
        unsafe {
            sys::ma_data_converter_uninit(&mut self.0);
        }
    }
}

unsafe impl Send for DataConverter {}
unsafe impl Sync for DataConverter {}
