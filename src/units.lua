
StaticLibrary {
	Name = "zlib",
	Sources = { Glob { Dir = "thirdparty/zlib-1.2.5", Extensions = { ".c" } } },
}

StaticLibrary {
	Name = "lua",
	Sources = { Glob { Dir = "thirdparty/lua-5.2.0-beta", Extensions = { ".c" } } },
}

Program {
	Name = "player",

	Env = {
		CPPPATH = { "hippoplayer/player" },
		CCOPTS = {
			{ "/MD", "/Od", "/Zi"; Config = "*-msvc-debug" },
		},

		PROGOPTS = {
			{ "/SUBSYSTEM:WINDOWS", "/DEBUG"; Config = { "win32-*-*", "win64-*-*" } },
		},
	},

	Libs = { 
		{ "user32.lib", "imm32.lib", "gdi32.lib", "ole32.lib", "version.lib", "shell32.lib", "Winmm.lib"; Config = "win*-*-*" }, 
	},

	Frameworks = { "Cocoa", "AudioToolbox", "CoreAudio" },

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

	Depends = { "zlib", "lua" },
}

Default "player"

