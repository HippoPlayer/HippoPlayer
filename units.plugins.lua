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
	Name = "hively",

	Includes = {
	    "src/plugin_api",
	},

	Sources = {
		"src/plugins/playback/hively/hively_plugin.cpp",
		"src/plugins/playback/hively/replayer/hvl_replay.c"
	},

	Depends = { "flatbuffers_lib" },
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

    Env = {
       CXXOPTS = {
			{ "-std=c++17"; Config = "linux-*-*" },
			{ "-std=c++17"; Config = "mac*-*-*" },
			{ "/std:c++17"; Config = "win64-*-*" },
		},
    },

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
		"src/plugins/playback/openmpt/openmpt_plugin.cpp",
	},

	Libs = {
		{ "stdc++"; Config = "macosx-*-*" },
		{ "Rpcrt4.lib"; Config = "win64-*-*" },
	},
}

local openmpt_cfg = CopyFile {
	Source = "src/plugins/playback/openmpt/libopenmpt.cfg",
	Target = "$(OBJECTDIR)/libopenmpt.cfg"
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
	Name = "nsf",

	Includes = {
	    "src/plugin_api",
	},

	Sources = {
        "src/plugins/playback/nsf/vcm/value.cpp",
        get_c_cpp_src("src/plugins/playback/nsf/xgm"),
		"src/plugins/playback/nsf/nsf_plugin.cpp",
	},

	Libs = {
		{ "Winmm.lib"; Config = "win64-*-*" },
	},

    Depends = { "zlib" },
}

-----------------------------------------------------------------------------------------------------------------------

StaticLibrary {
	Name = "sc68_file",

	SourceDir = "src/plugins/playback/sc68",

	Defines = {
        "HAVE_CONFIG_H",
	},

	Includes = {
        "src/plugins/playback/sc68/file68/sc68",
        "src/plugins/playback/sc68/unice68",
	},

	Sources = {
		-- File68
		"file68/src/error68.c",
		"file68/src/file68.c",
		"file68/src/gzip68.c",
		"file68/src/ice68.c",
		"file68/src/init68.c",
		"file68/src/vfs68.c",
		"file68/src/vfs68_ao.c",
		"file68/src/vfs68_curl.c",
		"file68/src/vfs68_fd.c",
		"file68/src/vfs68_file.c",
		"file68/src/vfs68_mem.c",
		"file68/src/vfs68_null.c",
		"file68/src/vfs68_z.c",
		"file68/src/msg68.c",
		"file68/src/option68.c",
		"file68/src/registry68.c",
		"file68/src/rsc68.c",
		"file68/src/string68.c",
		"file68/src/timedb68.c",
		"file68/src/uri68.c",
	},
}

-----------------------------------------------------------------------------------------------------------------------

SharedLibrary {
	Name = "sc68",

	SourceDir = "src/plugins/playback/sc68",

	Defines = {
        "HAVE_CONFIG_H",
	},

	Includes = {
	    "src/plugin_api",
        "src/plugins/playback/sc68/libsc68",
        "src/plugins/playback/sc68/file68/sc68",
        "src/plugins/playback/sc68/file68",
        "src/plugins/playback/sc68/libsc68/sc68",
        "src/plugins/playback/sc68/libsc68",
        "src/plugins/playback/sc68/libsc68/emu68",
	},

	Sources = {
		"libsc68/emu68/emu68.c",
		"libsc68/emu68/error68.c",
		"libsc68/emu68/getea68.c",
		"libsc68/emu68/inst68.c",
		"libsc68/emu68/ioplug68.c",
		"libsc68/emu68/mem68.c",
		"libsc68/emu68/line0_68.c",
		"libsc68/emu68/line1_68.c",
		"libsc68/emu68/line2_68.c",
		"libsc68/emu68/line3_68.c",
		"libsc68/emu68/line4_68.c",
		"libsc68/emu68/line5_68.c",
		"libsc68/emu68/line6_68.c",
		"libsc68/emu68/line7_68.c",
		"libsc68/emu68/line8_68.c",
		"libsc68/emu68/line9_68.c",
		"libsc68/emu68/lineA_68.c",
		"libsc68/emu68/lineB_68.c",
		"libsc68/emu68/lineC_68.c",
		"libsc68/emu68/lineD_68.c",
		"libsc68/emu68/lineE_68.c",
		"libsc68/emu68/lineF_68.c",
		"libsc68/emu68/table68.c",
		"libsc68/dial68/dial68.c",
		"libsc68/dial68/dial_conf.c",
		"libsc68/dial68/dial_tsel.c",
		"libsc68/dial68/dial_finf.c",
		"libsc68/src/api68.c",
		"libsc68/src/conf68.c",
		"libsc68/src/libsc68.c",
		"libsc68/src/mixer68.c",
		"libsc68/io68/io68.c",
		"libsc68/io68/mfp_io.c",
		"libsc68/io68/mfpemul.c",
		"libsc68/io68/mw_io.c",
		"libsc68/io68/mwemul.c",
		"libsc68/io68/paula_io.c",
		"libsc68/io68/paulaemul.c",
		"libsc68/io68/shifter_io.c",
		"libsc68/io68/ym_envel.c",
		"libsc68/io68/ym_blep.c",
		"libsc68/io68/ym_dump.c",
		"libsc68/io68/ym_io.c",
		"libsc68/io68/ym_puls.c",
		"libsc68/io68/ymemul.c",
		--"unice68/unice68.c",
		"unice68/unice68_pack.c",
		"unice68/unice68_unpack.c",
		"unice68/unice68_version.c",
		"sc68_plugin.cpp",
	},

    Depends = { "zlib", "sc68_file" },
}

-----------------------------------------------------------------------------------------------------------------------

SharedLibrary {
	Name = "tfmx",

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
	Name = "adplug",

	Includes = {
	    "src/plugin_api",
	    "src/plugins/playback/adplug/adplug_lib/libbinio/src",
	    "src/plugins/playback/adplug/adplug_lib/src",
	},

	Defines = {
        { "stricmp=strcasecmp" ; Config = { "linux-*-*", "mac*-*-*" } },
	},

	Sources = {
		get_c_cpp_src("src/plugins/playback/adplug"),
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
	Name = "mdx",

	Includes = {
	    "src/plugin_api",
	},

	Sources = {
		get_c_cpp_src("src/plugins/playback/mdx"),
	},
}

-----------------------------------------------------------------------------------------------------------------------

SharedLibrary {
	Name = "sid",
	Defines = {
		"HAVE_CXX11",
		"PACKAGE_NAME=\"\"",
		"PACKAGE_VERSION=\"\"",
		"PACKAGE_URL=\"\"",
	},

	Env	= {
       CXXOPTS = {
			{ "-Wno-deprecated-declarations"; Config = "linux-*-*" },
		},
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

SharedLibrary {
	Name = "player",
	Sources = {
	    "src/plugins/view/player/player.cpp",
	    "src/plugins/view/player/scrolltext.cpp",
        gen_moc("src/plugins/view/player/player.h"),
        gen_moc("src/plugins/view/player/scrolltext.h"),
	},

	Libs = {
		{ "wsock32.lib", "kernel32.lib", "user32.lib", "gdi32.lib", "Comdlg32.lib",
		  "Advapi32.lib", "Qt5Guid.lib", "Qt5Cored.lib", "Qt5Widgetsd.lib"; Config = "win64-*-*" },
	},

    Frameworks = { "Cocoa", "QtWidgets", "QtGui", "QtCore" },

	Depends = { "flatbuffers_lib" },
}

-----------------------------------------------------------------------------------------------------------------------

SharedLibrary {
	Name = "tracker",
	Sources = {
	    "src/plugins/view/tracker/tracker.cpp",
	    "src/plugins/view/tracker/tracker_display.cpp",
        gen_moc("src/plugins/view/tracker/tracker.h"),
        gen_moc("src/plugins/view/tracker/tracker_display.h"),
	},

	Libs = {
		{ "wsock32.lib", "kernel32.lib", "user32.lib", "gdi32.lib", "Comdlg32.lib",
		  "Advapi32.lib", "Qt5Guid.lib", "Qt5Cored.lib", "Qt5Widgetsd.lib"; Config = "win64-*-*" },
	},

    Frameworks = { "Cocoa", "QtWidgets", "QtGui", "QtCore" },

	Depends = { "flatbuffers_lib" },
}

-----------------------------------------------------------------------------------------------------------------------

SharedLibrary {
	Name = "playlist",
	Sources = {
	    "src/plugins/view/playlist/playlist.cpp",
        gen_moc("src/plugins/view/playlist/playlist.h"),
	},

    Frameworks = { "Cocoa", "QtWidgets", "QtGui", "QtCore" },

	Libs = {
		{ "wsock32.lib", "kernel32.lib", "user32.lib", "gdi32.lib", "Comdlg32.lib",
		  "Advapi32.lib", "Qt5Guid.lib", "Qt5Cored.lib", "Qt5Widgetsd.lib"; Config = "win64-*-*" },
	},
}

-----------------------------------------------------------------------------------------------------------------------

SharedLibrary {
	Name = "music_info",
	Sources = {
	    "src/plugins/view/music_info/music_info.cpp",
        gen_moc("src/plugins/view/music_info/music_info.h"),
	},

	Libs = {
		{ "wsock32.lib", "kernel32.lib", "user32.lib", "gdi32.lib", "Comdlg32.lib",
		  "Advapi32.lib", "Qt5Guid.lib", "Qt5Cored.lib", "Qt5Widgetsd.lib"; Config = "win64-*-*" },
	},

    Frameworks = { "Cocoa", "QtWidgets", "QtGui", "QtCore" },

	Depends = { "flatbuffers_lib" },
}



-----------------------------------------------------------------------------------------------------------------------
-- Default plugins
-----------------------------------------------------------------------------------------------------------------------

-- Tools

-- Default "flatc"

-- Decoders

Default "tfmx"
Default "hively"

Default "openmpt"
Default(openmpt_cfg)

Default "vgm"
Default "nsf"
Default "sc68"
Default "mdx"
Default "sid"
Default "adplug"

-- Views

--Default "playlist"
Default "player"
Default "playlist"
Default "music_info"

--Default "tracker"
--Default "song_info"
--Default "TestViewPlugin"

-- Default "FutureComposerPlugin"
--
-- vim: ts=4:sw=4:sts=4

