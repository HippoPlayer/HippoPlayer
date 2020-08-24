use cc;

fn main() {
    cc::Build::new().file("src/c_api/miniaudio.c").compile("miniaudio");
    println!("cargo:rerun-if-changed=./src/c_api/miniaudio.h");
    println!("cargo:rerun-if-changed=./src/c_api/miniaudio.c");
    println!("cargo:rerun-if-env-changed=CC");
}

