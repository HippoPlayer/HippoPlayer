-- vim: noexpandtab ts=4 sw=4

local files = require "tundra.syntax.files"
local dirwalk  = require "tundra.dirwalk"
local path = require "tundra.path"
local platform = require "tundra.platform"
local os = require "os"

local common_includes = {
	"src/plugin_api",
	"src/plugins/playback/uade/bencode/include",
	"src/plugins/playback/uade/uade/",
	"src/plugins/playback/uade/uade/src/",
	"src/plugins/playback/uade/uade/src/include/",
	"src/plugins/playback/uade/uade/src/frontends/include/",
	"src/plugins/playback/uade/uade/src/frontends/common/",
}

local source_dir = "src/plugins/playback/uade"
local macos_linux_opts = {
	Env = {
		CCOPTS = { {
			"-Wno-unused-variable" ; Config = "macosx-*-*" },
		},
	},
}

local build68k = Program {
	Includes = common_includes,
	SourceDir = source_dir,
	Name = "uade-build68k",
	Pass = "BuildTools",
	Sources = {
		"uade/src/build68k.c",
	},
}

DefRule {
	Name = "UaeBuild68k",
	Pass = "BuildTools",
	Command = "$(OBJECTDIR)$(SEP)uade-build68k < $(<) > $(@)",
	ImplicitInputs = { "$(OBJECTDIR)/uade-build68k$(PROGSUFFIX)" },
	Blueprint = { },
	Setup = function (env, data)
		return {
			InputFiles = { source_dir .. "/uade/src/table68k" },
			OutputFiles = { "$(OBJECTDIR)/cpudefs.c" },
		}
	end,
}

local cpudefs_cpp = UaeBuild68k {}

local gencpu = Program {
	Includes = common_includes,
	SourceDir = source_dir,

	Name = "uade-gencpu",
	Pass = "BuildTools",
	Sources = {
		"uade/src/missing.c",
		"uade/src/readcpu.c",
		"uade/src/gencpu.c",
		cpudefs_cpp,
	},
}

DefRule {
	Name = "UaeGenCpu",
	Pass = "BuildTools",
	Command = "cd $(@:[1]:D) && .$(SEP)uade-gencpu$(PROGSUFFIX)",
	ImplicitInputs = { "$(OBJECTDIR)/uade-gencpu$(PROGSUFFIX)" },
	Blueprint = { },
	Setup = function (env, data)
		return {
			InputFiles = { },
			OutputFiles = {
				"$(OBJECTDIR)/cputbl.h",
				"$(OBJECTDIR)/cpustbl.c",
				"$(OBJECTDIR)/cpuemu.c",
			},
		}
	end,
}

SharedLibrary {
	Name = "uade_plugin",

	--

	Env = {
		CCOPTS = { {
        	"-Wno-format",
        	"-Wno-unused-function",
        	"-Wno-unused-label",
			"-Wno-unused-variable" ; Config = "macosx-*-*" },
		},
	},

	Includes = common_includes,

	Defines = {
		-- These are for the cpu generation and was made for low memory. We just compile them all at once now instead
		"PART_1", "PART_2", "PART_3", "PART_4", "PART_5", "PART_6", "PART_7", "PART_8",
	},

	SourceDir = source_dir,

	Sources = {
        "uade/src/frontends/common/unixatomic.c",
        "uade/src/frontends/common/uadeipc.c",
        "uade/src/frontends/common/amifilemagic.c",
        "uade/src/frontends/common/eagleplayer.c",
        -- "uade/src/frontends/common/unixwalkdir.c",
        "uade/src/frontends/common/effects.c",
        "uade/src/frontends/common/uadecontrol.c",
        "uade/src/frontends/common/uadeconf.c",
        "uade/src/frontends/common/uadestate.c",
        "uade/src/frontends/common/uadeutils.c",
        "uade/src/frontends/common/md5.c",
        "uade/src/frontends/common/ossupport.c",
        "uade/src/frontends/common/rmc.c",
        "uade/src/frontends/common/songdb.c",
        "uade/src/frontends/common/songinfo.c",
        "uade/src/frontends/common/vparray.c",
        "uade/src/frontends/common/support.c",
        "uade/src/frontends/common/fifo.c",
        { "uade/compat/strlrep.c"; Config = "linux-*-*" },
        "uade/src/newcpu.c",
        "uade/src/memory.c",
        "uade/src/custom.c",
        "uade/src/cia.c",
        "uade/src/audio.c",
        "uade/src/compiler.c",
        "uade/src/missing.c",
        "uade/src/sd-sound.c",
        "uade/src/md-support.c",
        "uade/src/cfgfile.c",
        "uade/src/fpp.c",
        "uade/src/debug.c",
        "uade/src/readcpu.c",
        "uade/src/uade.c",
        "uade/src/uademain.c",
        "uade/src/sinctable.c",
        "uade/src/text_scope.c",
        "uade/src/socketpair.c",

        "bencode/bencode.c",

        "uade_plugin.cpp",

		cpudefs_cpp,
		UaeGenCpu { },
	},

	Libs = {
		{ "stdc++"; Config = "macosx-*-*" },
		{ "wsock32.lib", "ws2_32.lib"; Config = "win64-*-*" },
	},

}

Default "uade_plugin"
