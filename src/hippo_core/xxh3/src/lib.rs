
extern "C" {
    fn xx3h_hash_bits(data: *const u8, len: i32) -> u64;
}

pub fn hash(data: &[u8]) -> u64 {
    unsafe {
        xx3h_hash_bits(data.as_ptr(), data.len() as i32)
    }
}
