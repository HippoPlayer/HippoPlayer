#!/bin/bash
cd ../rute/generator
cargo build && cd ../../hippoplayer && ../rute/generator/target/debug/rute_gen -a ../rute/generator/src/api.def -m ../rute/generator/src/qt_manual.cpp -c src/external/rute_cpp/c_api.h -q src/external/rute_cpp/rute_cpp.cpp -b src/external/rute_cpp/qt_api_gen.h -r src/external/rute/src/rute.rs -f src/external/rute/src/ffi_gen.rs && bin/macos/tundra2 macosx-clang-debug
# tundra2 macosx-clang-debug


