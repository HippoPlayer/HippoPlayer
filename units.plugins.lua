require "tundra.syntax.glob"
require "tundra.path"
require "tundra.util"
require "tundra.syntax.rust-cargo"

local native = require('tundra.native')

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

SharedLibrary {
	Name = "HivelyPlugin",
	Sources = {
		"src/plugins/hively/HivelyPlugin.c",
		"src/plugins/hively/replayer/hvl_replay.c"
	},
}

-----------------------------------------------------------------------------------------------------------------------

SharedLibrary {
	Name = "openmpt",
	Defines = { "LIBOPENMPT_BUILD" },
	Includes = {
		"src/plugins/openmpt/libopenmpt",
		"src/plugins/openmpt/libopenmpt/common"
	},

	Sources = {
		get_c_cpp_src("src/plugins/openmpt/libopenmpt/soundlib"),
		get_c_cpp_src("src/plugins/openmpt/libopenmpt/common"),
		get_c_cpp_src("src/plugins/openmpt/libopenmpt/sounddsp"),
		"src/plugins/openmpt/libopenmpt/libopenmpt/libopenmpt_c.cpp",
		"src/plugins/openmpt/libopenmpt/libopenmpt/libopenmpt_cxx.cpp",
		"src/plugins/openmpt/libopenmpt/libopenmpt/libopenmpt_impl.cpp",
		"src/plugins/openmpt/libopenmpt/libopenmpt/libopenmpt_ext_impl.cpp",
		"src/plugins/openmpt/OpenMPT.cpp",
	},

	Libs = {
		{ "stdc++"; Config = "macosx-*-*" },
		{ "Rpcrt4.lib"; Config = "win64-*-*" },
	},
}

-----------------------------------------------------------------------------------------------------------------------

-- Program {
SharedLibrary {
	Name = "vgm",

	SourceDir = "src/plugins/vgm",

	Defines = {
		"DISABLE_HWOPL_SUPPORT",
		"ENABLE_ALL_CORES",
		"VGM_LITTLE_ENDIAN",
		"DISABLE_HW_SUPPORT",
	},

	Includes = {
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
	Name = "SidPlugin",
	Defines = { "HAVE_CXX11" },

	Includes = {
		"src/plugins/sid/libsidplayfp/src",
		"src/plugins/sid/libsidplayfp/src/builders/residfp-builder/residfp",
	},

	Sources = {
		get_c_cpp_src("src/plugins/sid"),
	},

	Libs = { "stdc++"; Config = "macosx-*-*" },
}

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

-----------------------------------------------------------------------------------------------------------------------

SharedLibrary {
	Name = "TfmxPlugin",

	Sources = {
		get_c_cpp_src("src/plugins/tfmx/src"),
	},
}


-----------------------------------------------------------------------------------------------------------------------
--[[
RustCrate {
	Name = "dummy",
	CargoConfig = "api/rust/prodbg/Cargo.toml",
	Sources = {
		get_rs_src("api/rust/dummy"),
	},
}
-----------------------------------------------------------------------------------------------------------------------

RustSharedLibrary {
	Name = "amiga_uae_plugin",
	CargoConfig = "src/addons/amiga_uae_plugin/Cargo.toml",
	Sources = {
		get_rs_src("src/addons/amiga_uae_plugin"),
		get_rs_src("src/crates/amiga_hunk_parser"),
		get_rs_src("src/crates/gdb-remote"),
		get_rs_src("api/rust/prodbg"),
	}
}
--]]

--Default "amiga_uae_plugin"
--Default "memory_view_2"
--Default "dummy_backend_plugin"

-- vim: ts=4:sw=4:sts=4

Default "HivelyPlugin"
Default "openmpt"
Default "vgm"
-- Default "FutureComposerPlugin"
-- Default "TfmxPlugin"
Default "SidPlugin"

