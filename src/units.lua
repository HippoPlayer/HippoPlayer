
StaticLibrary {
	Name = "zlib",
	Sources = { Glob { Dir = "thirdparty/zlib-1.2.5", Extensions = { ".c" } } },
}

Program {
	Name = "player",

	Env = {
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

	Frameworks = { "Cocoa", "AudioToolbox", "CoreAudio" },

	Sources = { "hippoplayer/player/main.c" },
	Depends = { "zlib" },
}

Default "player"

