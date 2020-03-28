#!/bin/bash
bin/macos/tundra2 macosx-clang-debug-asan

# this is to post patch up hippo_player with the correct run-time library for Rust

libpath="`rustc --print sysroot`";
libpath=$libpath$"/lib/libstd*";
libname_full="`ls $libpath`";
libname_only="${libname_full##*/}"
install_name_tool -change @rpath/$libname_only $libname_full t2-output/macosx-clang-debug-asan/hippo_player
