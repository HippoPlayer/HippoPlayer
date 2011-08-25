
StaticLibrary {
	Name = "zlib",
	Sources = { Glob { Dir = "thirdparty/zlib-1.2.5", Extensions = { ".c" } } },
}

StaticLibrary {
	Name = "lua",
	Sources = { Glob { Dir = "thirdparty/lua-5.2.0-beta", Extensions = { ".c" } } },
}

StaticLibrary {
	Name = "stb_image",
	Sources = { "thirdparty/stb_image/stb_image.c" },
}

-- A bit temporary but for testing now (should be a proper plugin later one)

StaticLibrary {
	Name = "HivelyPlugin",
	Sources = { 
		"hippoplayer/plugins/hively/HivelyPlugin.c", 
		"hippoplayer/plugins/hively/replayer/hvl_replay.c"
	},
}

Program {
	Name = "player",

	Env = {
		CPPPATH = { "hippoplayer/player", "thirdparty" },
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
			Dir = "hippoplayer/player",
			Extensions = { ".c", ".m" },
			Filters = {
				{ Pattern = "windows"; Config = "win32-*-*" },
				{ Pattern = "macosx"; Config = "macosx-*-*" },
			},
		},
	},

	Depends = { "zlib", "lua", "stb_image", "HivelyPlugin" },
}

Default "player"

