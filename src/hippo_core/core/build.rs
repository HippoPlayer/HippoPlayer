use std::env;

fn main() {
    let env = env::var("TARGET").unwrap();
    if env.contains("darwin") {
        println!("cargo:rustc-link-lib=c++");
    } else if env.contains("linux") {
        println!("cargo:rustc-link-lib=static=stdc++");
    }
}
