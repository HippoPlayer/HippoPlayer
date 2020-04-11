use serde_lexpr::{from_str, to_string};
use serde_derive::{Serialize, Deserialize};
use config::Config;

#[derive(Serialize, Deserialize, PartialEq, Eq, Default)]
struct Test {
    playback_priority_reverse: Vec<String>,
    playback_priority_forward: Vec<String>,
}

fn main() {
    let mut config = Test::default();
    config.playback_priority_reverse.push("uade".to_owned());
    config.playback_priority_reverse.push("uade 2".to_owned());
    config.playback_priority_forward.push("openmpt".to_owned());

    //let t = to_string(&config).unwrap();
    let t = "((playback_priority_forward \"uade\" \"uade 2\") (foo \"test\") (playback_priority_reverse \"openmpt\"))";

    let v: Test = from_str(&t).unwrap();

    println!("{}", t);
    println!("{}", v.playback_priority_reverse[0]);
}
