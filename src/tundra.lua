
Build {
	Units = "units.lua",

	SyntaxExtensions = { "tundra.syntax.glob" },

	Configs = {
		{
			Name = "macosx-clang",
			DefaultOnHost = "macosx",
			Tools = { "clang-osx" },
			Env = {
				CPPDEFS = { "HIPPO_MACOSX" },
				CCOPTS = {
                    { "-g", "-O0" ; Config = { "*-gcc-debug", "*-clang-debug" } }					
        		},
				CXXOPTS = {
					{ "-g", "-O0"; Config = "macosx-clang-debug" },
				},
			},
		},
	},
}
