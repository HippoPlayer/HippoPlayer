use cc;

fn main() {
    cc::Build::new().file("src/log.cpp").compile("log");
}

