
StaticLibrary {
	Name = "zlib",
	Sources = { Glob { Dir = "thirdparty/zlib-1.2.5", Extensions = { ".c" } } },
}

StaticLibrary {
	Name = "sdl",

	Env = {
		CPPPATH = { "thirdparty/SDL-1.3.0-5605/include" },
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
	Sources = { "hippoplayer/player/main.c" },
	Depends = { "zlib", "sdl" },
}

Default "player"

