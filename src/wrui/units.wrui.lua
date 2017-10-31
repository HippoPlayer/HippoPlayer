-- vim: expandtab ts=4 sw=4

require "tundra.syntax.glob"

local function get_src(dir, recursive)
    return FGlob {
        Dir = dir,
        Extensions = { ".cpp", ".c", ".h", ".s", ".m", ".sl" },
        Filters = {
            { Pattern = "[/\\]test[/\\]"; Config = "test-*" }, -- Directories named "test" and their subdirectories will be excluded from builds
            { Pattern = "%.s$"; Config = "amiga-*" },
            { Pattern = "[/\\]amiga[/\\]"; Config = "amiga-*" },
            { Pattern = "[/\\]windows[/\\]"; Config = "win32-*" },
            { Pattern = "[/\\]win32[/\\]"; Config = "win32-*" },
            { Pattern = "[/\\]mac[/\\]"; Config = "mac*-*" },
            { Pattern = "[/\\]linux[/\\]"; Config = "linux*-*" },
        },
        Recursive = recursive and true or false,
    }
end

-- Used to generate the moc cpp files as needed for .h that uses Q_OBJECT for QtTool(s)

DefRule {
	Name = "MocGeneration",
	Pass = "CodeGeneration",
	Command = "$(QT5)/bin/moc $(<) -o $(@)",

	Blueprint = {
		Source = { Required = true, Type = "string", Help = "Input filename", },
		OutName = { Required = true, Type = "string", Help = "Output filename", },
	},

	Setup = function (env, data)
		return {
			InputFiles    = { data.Source },
			OutputFiles   = { "$(OBJECTDIR)/_generated/" .. data.OutName },
		}
	end,
}

-- Used to send a list of header files

local function MocGenerationMulti(sources)
 local result = {}
 for _, src in ipairs(tundra.util.flatten(sources)) do
   result[#result + 1] = MocGeneration { Source = src, OutName = tundra.path.get_filename_base(src) .. "_moc.cpp" }
 end
 return result
end

SharedLibrary {
    Name = "wrui_qt",

    Env = {
        CPPPATH = {
            { "$(QT5)/lib/QtWidgets.framework/Headers",
              "$(QT5)/lib/QtCore.framework/Headers"; Config = "macosx-*-*" },
        },

        CXXOPTS = {
            { "-F$(QT5)/lib"; Config = "macosx-*-*" },
        },

        SHLIBCOM = {
            {  "-Wl,-rpath,$(QT5)/lib", "-F$(QT5)/lib", "-lstdc++", Config = "macosx-clang-*" },
            {  "-Wl,-rpath,$(QT5)/lib", "-lstdc++", "-lm", Config = "linux-*-*" },
        },

        FRAMEWORKS = { "QtCore", "QtWidgets" },
    },

    Sources = {
		MocGenerationMulti {
			Glob {
				Dir = "src/wrui/src/qt",
				Extensions = { ".h" }
			},
		},

        get_src("src/wrui/src/qt")
    },
}

Default "wrui_qt"
