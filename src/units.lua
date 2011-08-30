
SharedLibrary {
	Name = "HivelyPlugin",
	Sources = { 
		"plugins/hively/HivelyPlugin.c", 
		"plugins/hively/replayer/hvl_replay.c"
	},
}

Program {
	Name = "player",

	Env = {
		CPPPATH = { "hippoplayer", "hippoplayer/thirdparty", "hippoplayer/thirdparty/lua-5.2.0-beta/src"  },
		PROGOPTS = {
			{ "/SUBSYSTEM:WINDOWS", "/DEBUG"; Config = { "win32-*-*", "win64-*-*" } },
		},

		CCOPTS = {
			{ "-Werror", "-pedantic-errors", "-Wall"; Config = "macosx-gcc-debug" },
		},
	},

	Libs = { 
		{ "user32.lib", "imm32.lib", "gdi32.lib", "ole32.lib", "version.lib", "shell32.lib", "Winmm.lib"; Config = "win*-*-*" }, 
	},

	Frameworks = { "Cocoa", "AudioUnit", "AudioToolbox", "CoreAudio" },

	Sources = { 
		FGlob {
			Dir = "hippoplayer",
			Extensions = { ".c", ".m" },
			Filters = {
				{ Pattern = "windows"; Config = "win32-*-*" },
				{ Pattern = "macosx"; Config = "macosx-*-*" },
			},
		},
	},

	-- Depends = { "zlib", "lua", "stb_image" },
}

Default "player"

