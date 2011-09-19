
SharedLibrary {
	Name = "HivelyPlugin",
	Sources = { 
		"plugins/hively/HivelyPlugin.c", 
		"plugins/hively/replayer/hvl_replay.c"
	},
}

-- Flac plugin

SharedLibrary {
	Name = "FlacPlugin",
	Env = {
		CPPPATH = { "plugins/flac/libflac/src/libFLAC/include", "plugins/flac/libflac/include"  },
		CPPDEFS = {
			{ "DNDEBUG", "HAVE_STDINT_H", "HAVE_ICONV", "HAVE_CXX_VARARRAYS", "HAVE_LANGINFO_CODESET", -- "FLAC__HAS_OGG", 
			"_LARGEFILE_SOURCE", "_FILE_OFFSET_BITS=64", "FLAC__SYS_DARWIN", "WORDS_BIGENDIAN", "FLAC__INLINE=__inline__", -- 'VERSION=\"1.2.\"', 
			"FLAC__ALIGN_MALLOC_DATA"; Config = "macosx-*-*" }
		},
			
		CCOPTS = { 
			{ "-fomit-frame-pointer", "-funroll-loops", "-finline-functions", "-W", "-Wall", 
			"-Wmissing-prototypes", "-Wstrict-prototypes", "-Winline"; Config = "macosx-*-*" },
		},
	},

	Libs = { 
		{ "ogg"; Config = "macosx-*-*" }, 
	},

	Sources = { 
		Glob {
			Dir = "plugins/flac/libflac/src/libFLAC",
			Extensions = { ".c" },
		},

		-- "plugins/flac/FlacPlugin.c"
	},
}

-- Main player

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
		{ "user32.lib", "Msimg32.lib", "Comdlg32.lib", "imm32.lib", "gdi32.lib", "ole32.lib", "version.lib", "shell32.lib", "Winmm.lib"; Config = "win*-*-*" }, 
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

	-- Depends = { "FlacPlugin" },
}

Default "player"

