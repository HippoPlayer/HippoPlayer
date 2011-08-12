Build {
	Units = "units.lua",

	SyntaxExtensions = { "tundra.syntax.glob" },

	Configs = {
		{
			Name = "macosx-gcc",
			DefaultOnHost = "macosx",
			Tools = { "gcc" },
		},
		{
			Name = "win32-msvc",
			DefaultOnHost = "windows",
			Tools = { "msvc-vs2008" },
		},
	},
}
