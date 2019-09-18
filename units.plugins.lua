require "tundra.syntax.glob"
require "tundra.path"
require "tundra.util"
require "tundra.syntax.rust-cargo"

local native = require('tundra.native')

-----------------------------------------------------------------------------------------------------------------------

local function gen_moc(src)
    return Moc {
        Pass = "GenerateSources",
        Source = src
    }
end

-----------------------------------------------------------------------------------------------------------------------

local function get_rs_src(dir)
	return Glob {
		Dir = dir,
		Extensions = { ".rs" },
		Recursive = true,
	}
end

-----------------------------------------------------------------------------------------------------------------------

local function get_c_cpp_src(dir)
	return Glob {
		Dir = dir,
		Extensions = { ".cpp", ".c" },
		Recursive = true,
	}
end

-----------------------------------------------------------------------------------------------------------------------

StaticLibrary {
	Name = "zlib",

	Defines = {
		{ "WINDOWS"; Config = "win64-*-*" },
	},

	Env = {
		CCOPTS = {
			{ "-Wno-implicit-function-declaration", "-Wno-shift-negative-value"; Config = "mac*-*-*" },
			{ "/wd4996", "/wd4131"; Config = "win64-*-*" },
		}
	},

	SourceDir = "src/external/zlib",
	Sources = {
		"adler32.c",
		"compress.c",
		"crc32.c",
		"deflate.c",
		"gzclose.c",
		"gzlib.c",
		"gzread.c",
		"gzwrite.c",
		"infback.c",
		"inffast.c",
		"inflate.c",
		"inftrees.c",
		"trees.c",
		"uncompr.c",
		"zutil.c",
	},
}

-----------------------------------------------------------------------------------------------------------------------

StaticLibrary {
	Name = "flatbuffers_lib",

	Pass = "GenerateSources",

	SourceDir = "src/external/flatbuffers",

	Includes = {
		"src/external/flatbuffers/include",
	},

	Sources = {
		"src/code_generators.cpp",
		"src/idl_parser.cpp",
		"src/idl_gen_text.cpp",
		"src/reflection.cpp",
		"src/util.cpp",
	},
}

Program {
	Name = "flatc",

	Pass = "GenerateSources",

	SourceDir = "src/external/flatbuffers",

	Includes = {
		"src/external/flatbuffers/include",
		"src/external/flatbuffers",
	},

	Sources = {
		"src/idl_gen_cpp.cpp",
		"src/idl_gen_dart.cpp",
		"src/idl_gen_general.cpp",
		"src/idl_gen_kotlin.cpp",
		"src/idl_gen_go.cpp",
		"src/idl_gen_js_ts.cpp",
		"src/idl_gen_php.cpp",
		"src/idl_gen_python.cpp",
		"src/idl_gen_lobster.cpp",
		"src/idl_gen_lua.cpp",
		"src/idl_gen_rust.cpp",
		"src/idl_gen_fbs.cpp",
		"src/idl_gen_json_schema.cpp",
		"src/flatc.cpp",
		"src/flatc_main.cpp",
	},

	Depends = { "flatbuffers_lib" },
}

-----------------------------------------------------------------------------------------------------------------------

SharedLibrary {
	Name = "HivelyPlugin",

	Includes = {
	    "src/plugin_api",
	},

	Sources = {
		"src/plugins/playback/hively/HivelyPlugin.c",
		"src/plugins/playback/hively/replayer/hvl_replay.c"
	},

}

-----------------------------------------------------------------------------------------------------------------------

--[[
SharedLibrary {
	Name = "TestViewPlugin",

	Includes = {
	    "src/plugin_api",
	},

	Sources = {
		get_c_cpp_src("src/plugins/view/test_view"),
	},
}
--]]

-----------------------------------------------------------------------------------------------------------------------

SharedLibrary {
	Name = "openmpt",
	Defines = { "LIBOPENMPT_BUILD" },
	Includes = {
	    "src/plugin_api",
		"src/plugins/playback/openmpt/libopenmpt",
		"src/plugins/playback/openmpt/libopenmpt/common"
	},

	Sources = {
		get_c_cpp_src("src/plugins/playback/openmpt/libopenmpt/soundlib"),
		get_c_cpp_src("src/plugins/playback/openmpt/libopenmpt/common"),
		get_c_cpp_src("src/plugins/playback/openmpt/libopenmpt/sounddsp"),
		"src/plugins/playback/openmpt/libopenmpt/libopenmpt/libopenmpt_c.cpp",
		"src/plugins/playback/openmpt/libopenmpt/libopenmpt/libopenmpt_cxx.cpp",
		"src/plugins/playback/openmpt/libopenmpt/libopenmpt/libopenmpt_impl.cpp",
		"src/plugins/playback/openmpt/libopenmpt/libopenmpt/libopenmpt_ext_impl.cpp",
		"src/plugins/playback/openmpt/openmpt.cpp",
	},

	Libs = {
		{ "stdc++"; Config = "macosx-*-*" },
		{ "Rpcrt4.lib"; Config = "win64-*-*" },
	},
}

-----------------------------------------------------------------------------------------------------------------------

SharedLibrary {
	Name = "vgm",

	SourceDir = "src/plugins/playback/vgm",

	Defines = {
		"DISABLE_HWOPL_SUPPORT",
		"ENABLE_ALL_CORES",
		"VGM_LITTLE_ENDIAN",
		"DISABLE_HW_SUPPORT",
	},

	Includes = {
	    "src/plugin_api",
		"src/external/zlib",
	},


	Env = {
		CCOPTS = { {
			"-Wno-unused-value",
			"-Wno-unused-function",
			"-Wno-absolute-value", -- np_nes_dmc.c:536:6: warning: taking the absolute value of unsigned type 'unsigned int' has no effect [-We]
			"-Wno-parentheses-equality", -- es5503.c:149:12: warning: equality comparison with extraneous parentheses [-Wparentheses-equality]
			"-Wno-shift-negative-value", -- k054539.c:155:6: warning: shifting a negative signed value is undefined [-Wshift-negative-value]
			"-Wno-unused-variable" ; Config = "macosx-*-*" },
		},
	},

	Sources = {
		-- Taken from the Makefile
		"VGMPlay/chips/2151intf.c",
		"VGMPlay/chips/2203intf.c",
		"VGMPlay/chips/2413intf.c",
		"VGMPlay/chips/2608intf.c",
		"VGMPlay/chips/2610intf.c",
		"VGMPlay/chips/2612intf.c",
		"VGMPlay/chips/262intf.c",
		"VGMPlay/chips/3526intf.c",
		"VGMPlay/chips/3812intf.c",
		"VGMPlay/chips/8950intf.c",
		"VGMPlay/chips/Ootake_PSG.c",
		"VGMPlay/chips/adlibemu_opl2.c",
		"VGMPlay/chips/adlibemu_opl3.c",
		"VGMPlay/chips/ay8910.c",
		"VGMPlay/chips/ay8910_opl.c",
		"VGMPlay/chips/ay_intf.c",
		"VGMPlay/chips/c140.c",
		"VGMPlay/chips/c352.c",
		"VGMPlay/chips/c6280.c",
		"VGMPlay/chips/c6280intf.c",
		"VGMPlay/chips/dac_control.c",
		"VGMPlay/chips/emu2149.c",
		"VGMPlay/chips/emu2413.c",
		"VGMPlay/chips/es5503.c",
		"VGMPlay/chips/es5506.c",
		"VGMPlay/chips/fm.c",
		"VGMPlay/chips/fm2612.c",
		"VGMPlay/chips/fmopl.c",
		"VGMPlay/chips/gb.c",
		"VGMPlay/chips/iremga20.c",
		"VGMPlay/chips/k051649.c",
		"VGMPlay/chips/k053260.c",
		"VGMPlay/chips/k054539.c",
		"VGMPlay/chips/multipcm.c",
		"VGMPlay/chips/nes_apu.c",
		"VGMPlay/chips/nes_intf.c",
		"VGMPlay/chips/np_nes_apu.c",
		"VGMPlay/chips/np_nes_dmc.c",
		"VGMPlay/chips/np_nes_fds.c",
		"VGMPlay/chips/okim6258.c",
		"VGMPlay/chips/okim6295.c",
		"VGMPlay/chips/panning.c",
		"VGMPlay/chips/pokey.c",
		"VGMPlay/chips/pwm.c",
		"VGMPlay/chips/qsound.c",
		"VGMPlay/chips/rf5c68.c",
		"VGMPlay/chips/saa1099.c",
		"VGMPlay/chips/scd_pcm.c",
		"VGMPlay/chips/scsp.c",
		"VGMPlay/chips/scspdsp.c",
		"VGMPlay/chips/segapcm.c",
		"VGMPlay/chips/sn76489.c",
		"VGMPlay/chips/sn76496.c",
		"VGMPlay/chips/sn76496_opl.c",
		"VGMPlay/chips/sn764intf.c",
		"VGMPlay/chips/upd7759.c",
		"VGMPlay/chips/vsu.c",
		"VGMPlay/chips/ws_audio.c",
		"VGMPlay/chips/x1_010.c",
		"VGMPlay/chips/ym2151.c",
		"VGMPlay/chips/ym2413.c",
		"VGMPlay/chips/ym2413_opl.c",
		"VGMPlay/chips/ym2413hd.c",
		"VGMPlay/chips/ym2612.c",
		"VGMPlay/chips/ym3438.c",
		"VGMPlay/chips/ymdeltat.c",
		"VGMPlay/chips/ymf262.c",
		"VGMPlay/chips/ymf271.c",
		"VGMPlay/chips/ymf278b.c",
		"VGMPlay/chips/ymz280b.c",
		"VGMPlay/VGMPlay.c",
		"VGMPlay/VGMPlay_AddFmts.c",
		"VGMPlay/Stream.c",
		"VGMPlay/ChipMapper.c",
		"VGMPlay/vgm2wav.c",
		"vgm_plugin.cpp",
	},

	Libs = {
		{ "Winmm.lib"; Config = "win64-*-*" },
	},

    Depends = { "zlib" },
}

-----------------------------------------------------------------------------------------------------------------------

SharedLibrary {
	Name = "TfmxPlugin",

	Includes = {
	    "src/plugin_api",
	},

	Sources = {
		get_c_cpp_src("src/plugins/playback/tfmx"),
	},

	Libs = {
		{ "Wsock32.lib" ; Config = "win64-*-*" },
	},
}

-----------------------------------------------------------------------------------------------------------------------

SharedLibrary {
	Name = "DummyPlugin",

	Includes = {
	    "src/plugin_api",
	},

	Sources = {
		get_c_cpp_src("src/plugins/playback/dummy"),
	},
}

-----------------------------------------------------------------------------------------------------------------------

SharedLibrary {
	Name = "MDXPlugin",

	Includes = {
	    "src/plugin_api",
	},

	Sources = {
		get_c_cpp_src("src/plugins/playback/mdx"),
	},
}

-----------------------------------------------------------------------------------------------------------------------

SharedLibrary {
	Name = "SidPlugin",
	Defines = {
		"HAVE_CXX11",
		"PACKAGE_NAME=\"\"",
		"PACKAGE_VERSION=\"\"",
		"PACKAGE_URL=\"\"",
	},

	Includes = {
	    "src/plugin_api",
		"src/plugins/playback/sid/libsidplayfp/src",
		"src/plugins/playback/sid/libsidplayfp/src/builders/residfp-builder/residfp",
	},

	Sources = {
		get_c_cpp_src("src/plugins/playback/sid"),
	},

	Libs = { "stdc++"; Config = "macosx-*-*" },
}

--[[
-----------------------------------------------------------------------------------------------------------------------

SharedLibrary {
	Name = "FutureComposerPlugin",
	Env = {
		CPPPATH = { "src/plugins/futurecomposer/src" },
		CPPDEFS = {
			{ "HAVE_CONFIG_H"; Config = "macosx-*-*" }
		},
	},

	Sources = {
		get_c_cpp_src("src/plugins/futurecomposer/src"),
		"src/plugins/futurecomposer/FutureComposerPlugin.c",
	},

	Libs = {
		{ "stdc++"; Config = "macosx-*-*" },
	},
}

--]]

-----------------------------------------------------------------------------------------------------------------------
--  View plugins
-----------------------------------------------------------------------------------------------------------------------

--[[

RustSharedLibrary {
	Name = "playlist",
	CargoConfig = "src/plugins/view/playlist/Cargo.toml",
	Sources = {
		get_rs_src("src/plugins/view/playlist"),
		get_rs_src("src/plugin_api/rust/hippo_api"),
		get_rs_src("src/plugin_api/rust/messages"),
		get_rs_src("src/external/rute"),
	}
}

RustSharedLibrary {
	Name = "player",
	CargoConfig = "src/plugins/view/player/Cargo.toml",
	Sources = {
		get_rs_src("src/plugins/view/player"),
		get_rs_src("src/plugin_api/rust/hippo_api"),
		get_rs_src("src/plugin_api/rust/messages"),
		get_rs_src("src/external/rute"),
	}
}

RustSharedLibrary {
	Name = "song_info",
	CargoConfig = "src/plugins/view/song_info/Cargo.toml",
	Sources = {
		get_rs_src("src/plugins/view/song_info"),
		get_rs_src("src/plugin_api/rust/hippo_api"),
		get_rs_src("src/plugin_api/rust/messages"),
		get_rs_src("src/external/rute"),
	}
}

--]]

SharedLibrary {
	Name = "player",
	Sources = {
	    "src/plugins/view/player/player.cpp",
        gen_moc("src/plugins/view/player/player.h"),
	},

	Libs = {
		{ "wsock32.lib", "kernel32.lib", "user32.lib", "gdi32.lib", "Comdlg32.lib",
		  "Advapi32.lib", "Qt5Gui.lib", "Qt5Core.lib", "Qt5Widgets.lib"; Config = "win64-*-*" },
	},

	Depends = { "flatbuffers_lib" },
}

SharedLibrary {
	Name = "playlist",
	Sources = {
	    "src/plugins/view/playlist/playlist.cpp",
        gen_moc("src/plugins/view/playlist/playlist.h"),
	},

	Libs = {
		{ "wsock32.lib", "kernel32.lib", "user32.lib", "gdi32.lib", "Comdlg32.lib",
		  "Advapi32.lib", "Qt5Gui.lib", "Qt5Core.lib", "Qt5Widgets.lib"; Config = "win64-*-*" },
	},
}

-----------------------------------------------------------------------------------------------------------------------
-- Default plugins
-----------------------------------------------------------------------------------------------------------------------

-- Tools

Default "flatc"

-- Decoders

Default "TfmxPlugin"
Default "HivelyPlugin"
Default "openmpt"
Default "vgm"
Default "DummyPlugin"
Default "MDXPlugin"
Default "SidPlugin"

-- Views

--Default "playlist"
Default "player"
Default "playlist"
--Default "song_info"
--Default "TestViewPlugin"

-- Default "FutureComposerPlugin"
-- Default "TfmxPlugin"
--
-- vim: ts=4:sw=4:sts=4

