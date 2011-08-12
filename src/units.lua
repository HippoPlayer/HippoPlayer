
StaticLibrary {
	Name = "zlib",
	Sources = { Glob { Dir = "thirdparty/zlib-1.2.5", Extensions = { ".c" } } },
}

StaticLibrary {
	Name = "sdl",

	Env = {
		CPPPATH = { "thirdparty/sdl-1.2.14/include" },
	},
	
	Sources = {
		FGlob {
			Dir = "thirdparty/sdl-1.2.14",
			Extensions = { ".c" },
			Filters = {
				{ Pattern = "win32"; Config = "win32-*-*" },
				{ Pattern = "macosx"; Config = "macosx-*-*" },
				{ Pattern = "bsd"; Config = "bsd-*-*" },
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

