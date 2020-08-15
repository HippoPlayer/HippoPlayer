use std::env;

fn main() {
    let env = env::var("TARGET").unwrap();
    if env.contains("darwin") {
        println!("cargo:rustc-link-lib=c++");
    }
}
