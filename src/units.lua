
StaticLibrary {
	Name = "zlib",
	 Sources = { Glob { Dir = "thirdparty/zlib-1.2.5", Extensions = { ".c" } } },
}

Program {
	Name = "player",
	Sources = { "hippoplayer/player/main.c" },
	Depends = { "zlib" },
}

Default "player"

