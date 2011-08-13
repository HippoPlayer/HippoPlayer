
StaticLibrary {
	Name = "zlib",
	Sources = { Glob { Dir = "thirdparty/zlib-1.2.5", Extensions = { ".c" } } },
}

StaticLibrary {
	Name = "sdl",

	Env = {
		CPPPATH = { "thirdparty/SDL-1.3.0-5605/include" },
		CCOPTS = {
			{ "/MD"; Config = "*-msvc-debug" },
		}
	},
	
	Sources = {
		FGlob {
			Dir = "thirdparty/SDL-1.3.0-5605",
			Extensions = { ".c" },
			Filters = {
				{ Pattern = "windows"; Config = "win32-*-*" },
				{ Pattern = "macosx"; Config = "macosx-*-*" },
			}
		}
	}
}

Program {
	Name = "player",

	Env = {
		CPPPATH = { "thirdparty/SDL-1.3.0-5605/include" },
		CCOPTS = {
			{ "/MD"; Config = "*-msvc-debug" },
		},

		PROGOPTS = {
			{ "/SUBSYSTEM:WINDOWS"; Config = { "win32-*-*", "win64-*-*" } },
		},
	},

	Libs = { 
		{ "user32.lib", "imm32.lib", "gdi32.lib", "ole32.lib", "version.lib", "shell32.lib", "Winmm.lib"; Config = "win*-*-*" }, 
	},

	Sources = { "hippoplayer/player/main.c" },
	Depends = { "zlib", "sdl" },
}

Default "player"

