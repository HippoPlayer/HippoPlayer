use config::Config;

fn main() {
    let t = Config::from_file("/home/emoon/temp/test_cfg_2.cfg").unwrap();

    if let Some(list) = t.value.as_cons() {
        for v in list {
            dbg!(&v);
            println!("\n");
        }
    }
}
