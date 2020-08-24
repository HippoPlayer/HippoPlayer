pub mod error;
pub mod sys;
use logger::*;

pub use error::Error;
use std::mem::MaybeUninit;
use std::os::raw::c_char;
use std::os::raw::c_void;
pub use sys::ma_device;

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

impl Devices {
    pub fn new() -> Result<Devices, Error> {
        // backends prio, alsa over pulse audio and wasapi over dsound
        /*
        let backends: &[u32] = &[
            sys::ma_backend_alsa,
            sys::ma_backend_pulseaudio,
            sys::ma_backend_wasapi,
            sys::ma_backend_dsound,
        ];
        */

        let mut config = unsafe { sys::ma_context_config_init() };
        config.logCallback = Some(log_callback);
        config.pUserData = std::ptr::null_mut();

        let mut context = MaybeUninit::<sys::ma_context>::uninit();

        //let result = unsafe { sys::ma_context_init(backends.as_ptr(), 4, &config, context.as_mut_ptr()) };
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
                trace!(
                    "  Sample rate range {} - {}",
                    (*device).minSampleRate,
                    (*device).maxSampleRate
                );
            }

            //trace!("Found output device {}", name);

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
        device_config.playback.format = sys::ma_format_f32;
        device_config.playback.channels = 2;
        device_config.sampleRate = 48000;
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
}
