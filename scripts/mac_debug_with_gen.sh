#!/bin/bash
cargo build --manifest-path=../ui_gen/generator/Cargo.toml && ../ui_gen/generator/target/debug/ui_gen -a ../ui_gen/generator/src/api.def -m ../ui_gen/generator/src/qt_manual.cpp -c src/external/wrui_qt/c_api.h -q src/external/wrui_qt/wrui_qt.cpp -b src/external/wrui_qt/qt_api_gen.h -r src/external/wrui_rust/src/wrui.rs -f src/external/wrui_rust/src/ffi_gen.rs && bin/macos/tundra2 macosx-clang-debug
# tundra2 macosx-clang-debug


