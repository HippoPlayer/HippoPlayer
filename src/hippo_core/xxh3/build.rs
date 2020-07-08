use cc;

fn main() {
    cc::Build::new().file("src/xxh3.c").compile("xxh3");
}
