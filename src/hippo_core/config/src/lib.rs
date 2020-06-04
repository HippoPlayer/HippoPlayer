use lexpr::Value;
use serde::de::DeserializeOwned;
use serde_lexpr::from_str;
use std::fs::File;
use std::io::Read;
use std::path::Path;

pub struct Config {
    pub value: lexpr::Value,
}

impl Config {
    fn read_to_file<P: AsRef<Path>>(path: P) -> Result<(String, String), String> {
        let file_path = path.as_ref().to_str().unwrap().to_owned();

        // Open the file in read-only mode with buffer.
        let mut file = match File::open(&path) {
            Err(e) => return Err(format!("Unable to open {}: {}", file_path, e)),
            Ok(v) => v,
        };

        // Read file to to buffer but also include top level ( ) so files doesn't need to include that
        let mut data = String::new();
        data.push('(');

        match file.read_to_string(&mut data) {
            Err(e) => return Err(format!("Unable to read {}: {}", file_path, e)),
            Ok(_) => (),
        }

        data.push_str("\n)");
        Ok((file_path, data))
    }

    pub fn from_file<P: AsRef<Path>>(path: P) -> Result<Config, String> {
        let (file_path, data) = Self::read_to_file(path)?;

        // Read an arbitrary S-expression, using parser options suitable for Emacs Lisp.
        let value = match lexpr::from_str(&data) {
            Err(e) => return Err(format!("Unable to parse {}: {}", file_path, e)),
            Ok(v) => v,
        };

        Ok(Config { value })
    }

    ///
    /// This parses a file, inserts the extra ( ) for the outer scope and reports
    /// Parsing errors that may happen. This uses serde to deserialize into a struct of T
    ///
    pub fn deserialize<'a, P: AsRef<Path>, T: DeserializeOwned>(path: P) -> Result<T, String> {
        let (file_path, data) = Self::read_to_file(path)?;

        let t: T = match from_str(&data) {
            Err(e) => return Err(format!("Unable to parse {}: {}", file_path, e)),
            Ok(v) => v,
        };

        Ok(t)
    }

    ///
    /// Load a config file with a spec that allows more error checking to be done
    ///
    pub fn new_file_with_cfg<P: AsRef<Path>>(path: P, spec: &str) -> Result<Config, String> {
        let cfg = Self::from_file(path)?;
        // Read an arbitrary S-expression, using parser options suitable for Emacs Lisp.
        let _cfg_data = match lexpr::from_str(&spec) {
            Err(e) => return Err(format!("Unable to parse spec: {}", e)),
            Ok(v) => v,
        };

        Ok(cfg)
    }
}

/*
use lexpr::Value;
use std::error::Error;
use std::fs::File;
use std::io::BufReader;
use std::path::Path;
use std::io::prelude::*;
use std::collections::HashMap;
use std::fmt::Write;

pub struct Scfg {
    value: lexpr::Value,
}

impl Scfg {
    pub fn from_file<P: AsRef<Path>>(path: P) -> Result<Scfg, String> {
        // We reconstruct the error here to include the filename
        let file = match File::open(&path) {
            Err(e) => { return Err(format!("{} {}", path.as_ref().to_str().unwrap(), e)); }
            Ok(f) => f,
        };

        let reader = BufReader::new(&file);

        // Read an arbitrary S-expression, using parser options suitable for Emacs Lisp.
        let value = lexpr::from_reader_custom(reader, lexpr::parse::Options::elisp());

        match value {
            Err(e) => {
                let filename = path.as_ref().to_str().unwrap();

                // Style of error we want
                // /home/emoon/temp/test.c:14:8: note: to match this '('
                //     printf("%d %d - res %d\n", t0, t1, t0 >> t1
                //           ^

                if let Some(loc) = e.location() {
                    let file = File::open(&path).unwrap();
                    let reader = BufReader::new(&file);
                    let mut lines = HashMap::new();

                    for (i, line) in reader.lines().enumerate() {
                        println!("line {}", line.unwrap());
                        if let Ok(l) = line {
                            lines.insert(i + 1, l.to_owned());
                        }
                    }
                    let mut output = String::new();

                    println!("here");

                    dbg!(&lines);

                    writeln!(&mut output, "{}:{}:{}: {}", filename, loc.line(), loc.column(), e).unwrap();

                    /*
                    if let Some(line) = lines.get_key_value(&loc.line()) {
                        writeln!(output, "{}", line.1).unwrap();
                        writeln!(output, "{:width$}^", "", width = loc.column()).unwrap();
                    }
                    */

                    Err(output)
                } else {
                    Err(format!("{}: {}", filename, e))
                }
            },

            Ok(v) => Ok(Scfg { value: v }),
        }
    }
}


#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn test_file() {
        Scfg::from_file("test/test.cfg").unwrap();
    }
}
*/
