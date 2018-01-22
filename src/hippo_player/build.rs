use std::env;
use std::fs::File;
use std::io::Write;
use std::path::Path;

fn main() {
    let out_dir = env::var("OUT_DIR").unwrap();
    let dest_path = Path::new(&out_dir).join("build_id.rs");
    let build_id = env::var("BUILD_DATE_ID").unwrap_or("".to_owned());
    let mut f = File::create(&dest_path).unwrap();

    f.write_fmt(format_args!("pub fn build_id() -> &'static str {{ \"{}\" }}", build_id)).unwrap();
}
