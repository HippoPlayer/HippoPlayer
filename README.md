# HippoPlayer

HippoPlayer is a music player for macOS, Windows and Linux that mostly focuses on music made for systems such as Amiga, C64, and gaming systems. While HippoPlayer supports MP3, FLAC, Ogg Vorbis, etc it's not the main target.

## Motivation

The original HippoPlayer is something I used a lot when I had my Amiga. Over many years I tried various players and while some of them hold up none of them has been what I been looking for.
Before starting this project I looked around quite a bit for alternatives and nothing really fit the bill.

Closest is likely [XMPlay](https://www.xmplay.com/), but that's for Windows only and closed source which isn't what I want either but feature wise it's fairly good.
I started this project roughly 7 years ago. I did some work in a private repository and had something very basic up and running. Then other things happened and the project has been on ICE for quite many years but I always had it back in my head that I wanted to come back to it.

The aim I have with HippoPlayer is "a modern music player for your oldsk00l needs" This means it should feel nice and great to use with features that you can expect from a modern player but allow you to play older formats in the best way possible.

## Moving forward

The project has now been started up again and it's now useable but still very limited. The screenshot include here is a just for **testing**. This is **not** the final design of the UI (but it's nicer to have something useable instead of cmdline only :) A redesign of the UI is in the works.

![Screenshot](/bin/hippo_screenshot_new.png)

There are many plans for HippoPlayer moving forward:

* Very rich support for many formats. The current format list is available here https://github.com/emoon/HippoPlayer/issues/13
* Big focus on visualizing data including (but not limited to): tracker view, hw state for emulated hardware, samples, oscilloscope, piano view and more.
* A modern UI that is easy to use but still full of features.
* Deep integration with existing websites that provides content that HippoPlayer can use and/or show as part of playing music such as: Demozoo, SceneSat and Scenemusic/Nectarine. This list will certainly grow with customized integration for many of these.
* Easy access to music. Possibility to index and auto download music from from various (free) sources of music (such as [modland](ftp.modland.com)) and more.
* More to come here!

Early builds of HippoPlayer for Mac and Windows can be found [here](http://hippoplayer.s3.eu-west-3.amazonaws.com/list.html?sort=lastmod&sortdir=desc)

# Building the code

[![Build status](https://github.com/emoon/HippoPlayer/workflows/CI/badge.svg)](https://github.com/emoon/HippoPlayer/actions?query=workflow%3ACI)

HippoPlayer is being written in [Rust](https://www.rust-lang.org) and uses Qt for the UI. All playback plugins uses a C API which means that plugins may be written in C/C++ also

Rust 1.20+ Latest stable version of **Rust** (1.20+) needs to be present on the system. We recommend using [rustup](https://www.rustup.rs/) to install and manage your Rust toolchain(s). If you already have rustup installed but aren't on the latest stable Rust, you can simply run `rustup update`. When installing using rustup for the first time simply select the defaults (option 1).

### Linux

First install Tundra:

```
git clone --recursive https://github.com/deplinenoise/tundra.git && cd tundra && make -j2 && sudo make install && cd .. && rm -rfv tundra
```

Then download Qt 5.15. There are several methods, but they all depend on which distro you are using. You can go to the Qt homepage and follow instructions there to download the opensource version, using something like `apt-get` on Debian/Ubuntu style distros.
After installing Qt you need to set up 3 environment variables.

```
export QT5_BIN=$Qt5_DIR/bin
export QT5_INC=$Qt5_DIR/include
export QT5_LIB=$Qt5_DIR/lib
```

These in the case above `Qt5_DIR` would be something like `/path/to/Qt/5.15.0/gcc_64`

Then run `scripts/linux_build_debug.sh` and run `t2-output/linux-gcc-debug-default/hippo_player`

## macOS

Download Qt 5.15 (using homebrew or from the Qt website)

After installing Qt you need to set up 3 environment variables.

```
export QT5_BIN=$Qt5_DIR/bin
export QT5_INC=$Qt5_DIR/include
export QT5_LIB=$Qt5_DIR/lib
```

These in the case above `Qt5_DIR` would be something like `/path/to/Qt/5.15.0/clang_64`

Then run `scripts/mac_build_debug.sh` and run `t2-output/macosx-clang-debug-default/hippo_player`

## Windows

Download Qt 5.15 from the Qt website and Visual Studio 2019 (Community Edition works fine but Pro/Enterprise as well)

After installing Qt you need to set up 3 environment variables (either in the shell or in Windows environment variables for the user)

```
set QT5_BIN=c:\path\to\qt\5.15.0\msvc2017_64\bin
set QT5_LIB=c:\path\to\qt\5.15.0\msvc2017_64\lib
set QT5_INC=c:\path\to\qt\5.15.0\msvc2017_64\include
```

Then run `scripts\win64_build_debug.cmd` and run `t2-output\win64-msvc-debug-default\hippo_player`
if you get "missing symbol ..." then run `scripts\win64_copy_bins.cmd` and try to run again

On Windows it's also possible to run `scripts\win64_gen_vs_solution.cmd` followed by `scripts\win64_launch_vs.cmd` to open a generated VS solution to work in.

## Acknowledgement

The name HippoPlayer is reused with permission from Kari-Pekka Koljonen which made the original HippoPlayer for Amiga. The original code can be found [here](https://github.com/koobo/HippoPlayer)

## License

Licensed under either of

 * Apache License, Version 2.0 ([LICENSE-APACHE](LICENSE-APACHE) or http://www.apache.org/licenses/LICENSE-2.0)
 * MIT license ([LICENSE-MIT](LICENSE-MIT) or http://opensource.org/licenses/MIT)

at your option.

### Contribution

Unless you explicitly state otherwise, any contribution intentionally submitted for inclusion in the work by you, as defined in the Apache-2.0 license, shall be dual licensed as above, without any additional terms or conditions.

## Exception to the licence

Notice that each plugin may be under a different licence given the origin of the original code. Please check this first if you have concerns regarding this.


