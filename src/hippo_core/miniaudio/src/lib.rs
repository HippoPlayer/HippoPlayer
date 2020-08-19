pub mod error;
pub mod sys;

use error::Error;
use std::mem::MaybeUninit;
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
    // FIXME I don't think the behavior here is undefined since u8 should have an alignment of 1, but
    // I might be wrong :P

    let byte_len = orig.len() * std::mem::size_of::<T>();
    let ptr = orig.as_ptr() as *const u8;
    unsafe { std::slice::from_raw_parts(ptr, byte_len) }
}

pub fn cstr_display<'s>(full_arr: &'s [::std::os::raw::c_char]) -> std::borrow::Cow<'s, str> {
    let byte_slice = into_byte_slice(full_arr);

    if let Ok(cstr) = std::ffi::CStr::from_bytes_with_nul(byte_slice) {
        cstr.to_string_lossy()
    } else {
        // if there is no nul terminator, just use the entire slice:
        String::from_utf8_lossy(byte_slice)
    }
}

pub unsafe fn enumerate_devices() -> i32 {
    use std::ptr::null;

    let mut context = MaybeUninit::<sys::ma_context>::uninit();
    if sys::ma_context_init(null(), 0, null(), context.as_mut_ptr()) != sys::MA_SUCCESS as _ {
        eprintln!("Failed to initialize context.");
        return -2;
    }
    let mut context = context.assume_init();

    let mut playback_device_infos = MaybeUninit::<*mut sys::ma_device_info>::uninit();
    let mut playback_device_count = 0u32;

    let mut capture_device_infos = MaybeUninit::<*mut sys::ma_device_info>::uninit();
    let mut capture_device_count = 032;

    let result = sys::ma_context_get_devices(
        &mut context,
        playback_device_infos.as_mut_ptr(),
        &mut playback_device_count,
        capture_device_infos.as_mut_ptr(),
        &mut capture_device_count,
    );

    if result != sys::MA_SUCCESS as _ {
        eprintln!("Failed to retrieve device information.");
        return -3;
    }

    let playback_device_infos = playback_device_infos.assume_init();
    let capture_device_infos = capture_device_infos.assume_init();

    println!("Playback Devices:");
    for device_idx in 0..playback_device_count {
        let device = playback_device_infos.add(device_idx as usize);
        let name = cstr_display(&(*device).name);
        println!("\t{}: {}", device_idx, name);
    }

    println!("Capture Devices:");
    for device_idx in 0..capture_device_count {
        let device = capture_device_infos.add(device_idx as usize);
        let name = cstr_display(&(*device).name);
        println!("\t{}: {}", device_idx, name);
    }

    sys::ma_context_uninit(&mut context);

    return 0;
}

pub struct Device {
    device: *mut sys::ma_device,
}

unsafe extern "C" fn stop_callback(_device_ptr: *mut sys::ma_device) {
    println!("Device Stopped.");
}

impl Device {
    pub fn new(callback: DataCallback, user_data: *mut c_void) -> Result<Device, Error> {
        let device = Box::into_raw(Box::new(MaybeUninit::<sys::ma_device>::uninit()));

        let mut device_config = unsafe { sys::ma_device_config_init(sys::ma_device_type_playback) };

        device_config.playback.format = sys::ma_format_f32;
        device_config.playback.channels = 2;
        device_config.sampleRate = 48000;
        device_config.dataCallback = Some(callback);
        device_config.stopCallback = Some(stop_callback);
        device_config.pUserData = user_data;

        // We don't need low-latency audio, larger buffers is better for us
        device_config.performanceProfile = sys::ma_performance_profile_conservative;

        let result = unsafe {
            sys::ma_device_init(std::ptr::null_mut(), &device_config, (*device).as_mut_ptr())
        };

        map_result!(
            result,
            Device {
                device: unsafe { (*device).as_mut_ptr() }
            }
        )
    }

    pub fn start(&self) -> Result<(), Error> {
        unsafe { Error::from_c_result(sys::ma_device_start(self.device as *const _ as *mut _)) }
    }
}
