#bin/sh
bindgen --no-prepend-enum-name --no-layout-tests src/plugin_api/HippoPlugin.h > src/hippo_core/core/src/ffi_gen.rs
