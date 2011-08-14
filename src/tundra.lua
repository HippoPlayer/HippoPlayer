Build {
	Units = "units.lua",

	SyntaxExtensions = { "tundra.syntax.glob" },

	Configs = {
		{
			Name = "macosx-gcc",
			DefaultOnHost = "macosx",
			Tools = { "clang-osx" },
			Env = {
				CPPDEFS = { "HIPPO_MACOSX" },
			},
		},
		{
			Name = "win32-msvc",
			DefaultOnHost = "windows",
			Tools = { "msvc-vs2008" },
			Env = {
				CPPDEFS = { "HIPPO_WIN32" },
			},
		},
	},
}
