#/bin/bash
bindgen --no-layout-tests src/plugin_api/HippoPlugin.h > src/plugin_api/rust/hippo_api/src/ffi.rs
