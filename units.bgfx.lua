require "tundra.syntax.glob"
require "tundra.path"
require "tundra.util"
require "tundra.syntax.rust-cargo"

local native = require('tundra.native')

DefRule {
	Name = "ShadercFS",
	Command = "$(BGFX_SHADERC) -f $(<) -o $(@) --type fragment --platform " .. shaderc_platform .. shaderc_ps_extra_params,

	Blueprint = {
		Source = { Required = true, Type = "string", Help = "Input filename", },
		OutName = { Required = false, Type = "string", Help = "Output filename", },
	},

	Setup = function (env, data)
		return {
			InputFiles    = { data.Source },
			OutputFiles   = { "$(OBJECTDIR)/_generated/" .. path.drop_suffix(data.Source) .. ".fs" },
		}
	end,
}

DefRule {
	Name = "ShadercVS",
	Command = "$(BGFX_SHADERC) -f $(<) -o $(@) --type vertex --platform " .. shaderc_platform .. shaderc_vs_extra_params,

	Blueprint = {
		Source = { Required = true, Type = "string", Help = "Input filename", },
		OutName = { Required = false, Type = "string", Help = "Output filename", },
	},

	Setup = function (env, data)
		return {
			InputFiles    = { data.Source },
			OutputFiles   = { "$(OBJECTDIR)/_generated/" .. path.drop_suffix(data.Source) .. ".vs" },
		}
	end,
}

-----------------------------------------------------------------------------------------------------------------------

Program {
	Name = "bgfx_shaderc",
	Target = "$(BGFX_SHADERC)",
	Pass = "BuildTools",

    Env = {
        CCOPTS = {
			{ "/wd4291", "/W3", "-D__STDC__", "-D__STDC_VERSION__=199901L", "-Dstrdup=_strdup", "-Dalloca=_alloca", "-Disascii=__isascii"; Config = "win64-*-*" },
        	{ "-Wno-everything"; Config = "macosx-*-*" },
			{ "-fno-strict-aliasing"; Config = { "macosx-*-*", "linux-*-*" } },
        },

        CXXOPTS = {
			{ "/wd4291", "/W3", "-D__STDC__", "-D__STDC_VERSION__=199901L", "-Dstrdup=_strdup", "-Dalloca=_alloca", "-Disascii=__isascii"; Config = "win64-*-*" },
        	{ "-Wno-everything"; Config = "macosx-*-*" },
			{ "-fno-strict-aliasing"; Config = { "macosx-*-*", "linux-*-*" } },
        },

		CPPDEFS = {
			{ "NINCLUDE=64", "NWORK=65536", "NBUFF=65536", "OLD_PREPROCESSOR=0" },
		},

		CPPPATH = {
			{
				BX_DIR .. "include",
				BGFX_DIR .. "include",
				FCPP_DIR,
				GLSL_OPTIMIZER .. "src",
				GLSL_OPTIMIZER .. "include",
				GLSL_OPTIMIZER .. "src/mesa",
				GLSL_OPTIMIZER .. "src/mapi",
				GLSL_OPTIMIZER .. "src/glsl",
				GLSL_OPTIMIZER .. "src/glsl/glcpp",
			},

			{
				BX_DIR .. "include/compat/osx" ; Config = "macosx-*-*"
			},

			{
				BX_DIR .. "include/compat/msvc"; Config = "win64-*-*"
			},
		},

		PROGCOM = {
			{ "-lstdc++"; Config = { "macosx-clang-*", "linux-gcc-*" } },
			{ "-lm -lpthread -ldl -lX11"; Config = "linux-*-*" },
		},
	},

	Sources = {
		BGFX_DIR .. "tools/shaderc/shaderc.cpp",
		BGFX_DIR .. "tools/shaderc/shaderc_glsl.cpp",
		BGFX_DIR .. "tools/shaderc/shaderc_hlsl.cpp",
		BGFX_DIR .. "src/vertexdecl.cpp",
		BGFX_DIR .. "src/vertexdecl.h",

		FCPP_DIR .. "cpp1.c",
		FCPP_DIR .. "cpp2.c",
		FCPP_DIR .. "cpp3.c",
		FCPP_DIR .. "cpp4.c",
		FCPP_DIR .. "cpp5.c",
		FCPP_DIR .. "cpp6.c",

		FGlob {
			Dir = GLSL_OPTIMIZER .. "src/mesa",
			Extensions = { ".c", ".h" },
			Filters = {
				{ Pattern = "main.cpp", Config = "never" }
			}
		},
		FGlob {
			Dir = GLSL_OPTIMIZER .. "src/glsl",
			Extensions = { ".cpp", ".c", ".h" },
			Filters = {
				{ Pattern = "main.cpp", Config = "never" }
			}
		},
		FGlob {
			Dir = GLSL_OPTIMIZER .. "src/util",
			Extensions = { ".c", ".h" },
			Filters = {
				{ Pattern = "main.cpp", Config = "never" }
			}
		},
	},

    Libs = { { "kernel32.lib", "d3dcompiler.lib", "dxguid.lib" ; Config = "win64-*-*" } },

    Frameworks = {
        { "Cocoa" },
        { "Metal" },
        { "QuartzCore" },
        { "OpenGL" }
    },

	IdeGenerationHints = { Msvc = { SolutionFolder = "Tools" } },
}

StaticLibrary {
    Name = "bgfx_native",

    Env = {
        CPPPATH = {
		  {
		  	{ "src/native/external/bx/include/compat/msvc",
		  	  "src/native/external/bgfx/3rdparty/dxsdk/include" } ; Config = "win64-*-*" },
          { "src/native/external/bx/include/compat/osx" ; Config = "macosx-*-*" },
            "src/native/external/remotery/lib",
            "src/native/external/bgfx/include",
            "src/native/external/bx/include",
            "src/native/external/bgfx/3rdparty/khronos",
        },

        CXXOPTS = {
			{ "-Wno-variadic-macros", "-Wno-everything" ; Config = "macosx-*-*" },
			{ "/Isrc/native/external/bx/include/compat/msvc",
			"/EHsc"; Config = "win64-*-*" },
        },
    },

    Sources = {
		{ "src/native/external/bgfx/src/bgfx.cpp",
		  "src/native/external/bgfx/src/image.cpp",
		  "src/native/external/bgfx/src/vertexdecl.cpp",
		  "src/native/external/bgfx/src/debug_renderdoc.cpp",
		  "src/native/external/bgfx/src/topology.cpp",
		  "src/native/external/bgfx/src/shader_dxbc.cpp",
		  "src/native/external/bgfx/src/renderer_gl.cpp",
		  "src/native/external/bgfx/src/renderer_vk.cpp",
		  "src/native/external/bgfx/src/renderer_null.cpp",
		  "src/native/external/bgfx/src/renderer_d3d9.cpp",
		  "src/native/external/bgfx/src/renderer_d3d11.cpp",
		  "src/native/external/bgfx/src/renderer_d3d12.cpp" },
        { "src/native/external/bgfx/src/renderer_mtl.mm" ; Config = "macosx-*-*" },
	    { "src/native/external/bgfx/src/glcontext_wgl.cpp" ; Config = "win64-*-*" },
	    { "src/native/external/bgfx/src/glcontext_glx.cpp" ; Config = "linux-*-*" },
	    { "src/native/external/bgfx/src/glcontext_nsgl.mm" ; Config = "macosx-*-*" },
    },

	IdeGenerationHints = { Msvc = { SolutionFolder = "External" } },
}
