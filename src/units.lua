
SharedLibrary {
	Name = "HivelyPlugin",
	Sources = { 
		"plugins/hively/HivelyPlugin.c", 
		"plugins/hively/replayer/hvl_replay.c"
	},
}

-- Mikmod

SharedLibrary {
	Name = "MikmodPlugin",
	Env = {
		CPPPATH = { "plugins/mikmod/mikmod/include" },
	},

	Sources = { 
		Glob {
			Dir = "plugins/mikmod/mikmod",
			Extensions = { ".c" },
		},

		"plugins/mikmod/MikmodPlugin.c", 
	},

	Frameworks = { "AudioUnit", "CoreAudio" },
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

		LIBPATH = { "plugins/flac/libflac/lib" },
			
		CCOPTS = { 
			{ "-fomit-frame-pointer", "-funroll-loops", "-finline-functions", "-W", "-Wall", 
			"-Wstrict-prototypes", "-Winline"; Config = "macosx-*-*" },
		},
	},

	Libs = { 
		{ "ogg", "FLAC_1_2_1"; Config = "macosx-*-*" }, 
	},

	Sources = { 

		"plugins/flac/FlacPlugin.c"
	},
}

-- SidPlugin

SharedLibrary {
	Name = "SidPlugin",
	Env = {
		CPPPATH = { "plugins/sid/libsidplay2/include/sidplay",
					"sid/libsidplay2/include/sidplay/builders"},
		LIBPATH = { "plugins/sid/lib" },
	},

	Sources = { 
		"plugins/sid/Sidplugin.cpp", 
	},


	Libs = { 
		{ "sidplay2", "resid-builder", "stdc++"; Config = "macosx-*-*" }, 
	},
}

-- FutureComposer

SharedLibrary {
	Name = "FutureComposerPlugin",
	Env = {
		CPPPATH = { "plugins/futurecomposer/src" },
		CPPDEFS = {
			{ "HAVE_CONFIG_H"; Config = "macosx-*-*" }
		},
	},

	Sources = { 
		Glob {
			Dir = "plugins/futurecomposer/src",
			Extensions = { ".cpp" },
		},

		-- "plugins/futurecomposer/FutureComposerPlugin.c", 
	},

	Libs = { 
		{ "stdc++"; Config = "macosx-*-*" }, 
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
			{ "-Werror", "-pedantic-errors", "-Wall"; Config = "macosx-clang-*" },
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
				{ Pattern = "macosx"; Config = "macosx-*-*" },
			},
		},
	},
}

Default "player"
Default "HivelyPlugin"
Default "FlacPlugin"
Default "MikmodPlugin"
Default "SidPlugin"
Default "FutureComposerPlugin"

local hippoBundle = OsxBundle {
	Depends = { "player" },
	Target = "$(OBJECTDIR)/HippoPlayer.app",
	InfoPList = "hippodata/Info.plist",
	Executable = "$(OBJECTDIR)/player",
	Resources = {
		CompileNib { Source = "hippodata/appnib.xib", Target = "appnib.nib" },
		"hippodata/icon.icns",
	},
}

Default(hippoBundle)


