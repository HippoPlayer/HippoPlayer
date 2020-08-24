#/bin/sh
bindgen --whitelist-type "ma_.*" --whitelist-function "ma_.*" --whitelist-var "(ma|MA)_.*" --size_t-is-usize --impl-debug --no-layout-tests --no-prepend-enum-name src/c_api/miniaudio.h > src/bindings.rs
