require "tundra.syntax.glob"
require "tundra.path"
require "tundra.util"
require "tundra.syntax.rust-cargo"

local native = require('tundra.native')

local BIMG_DIR = "src/external/bimg/"
local BX_DIR = "src/external/bx/"
local BGFX_DIR = "src/external/bgfx/"
local GLFW_DIR = "src/external/glfw/"

local GLSL_OPTIMIZER = BGFX_DIR  .. "3rdparty/glsl-optimizer/"
local FCPP_DIR = BGFX_DIR .. "3rdparty/fcpp/"
local SPIRV_CROSS = BGFX_DIR .. "3rdparty/spirv-cross/"
local GLSLANG_DIR = BGFX_DIR .. "3rdparty/glslang/"
local SPIRV_TOOLS = BGFX_DIR .. "3rdparty/spirv-tools/"
local SPIRV_HEADERS = BGFX_DIR .. "3rdparty/spirv-headers/"

-- setup target for shader
local shaderc_platform = "windows"
local shaderc_vs_extra_params = " -p vs_5_0"
local shaderc_ps_extra_params = " -p ps_5_0"
if native.host_platform == "macosx" then
	shaderc_platform = "osx"
	shaderc_vs_extra_params = ""
	shaderc_ps_extra_params = ""
elseif native.host_platform == "linux" then
	shaderc_platform = "linux"
	shaderc_vs_extra_params = ""
	shaderc_ps_extra_params = ""
end

-----------------------------------------------------------------------------------------------------------------------

local function get_c_cpp_src(dir)
    return Glob {
        Dir = dir,
        Extensions = { ".cpp", ".c", ".h" },
        Recursive = true,
	}
end

local function glob_no_main(dir, ext, rec)
	return FGlob {
		Dir = dir,
		Extensions = { ext, ".h" },
		Filters = {
			{ Pattern = "main.cpp", Config = "never" }
		},
        Recursive = rec,
	}
end

-----------------------------------------------------------------------------------------------------------------------

local function glob_c_cpp_no_main(dir)
	return FGlob {
		Dir = dir,
		Extensions = { ".cpp", ".c", ".h" },
		Filters = {
			{ Pattern = "main.cpp", Config = "never" }
		},
        Recursive = false,
	}
end

-----------------------------------------------------------------------------------------------------------------------

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

-----------------------------------------------------------------------------------------------------------------------

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

StaticLibrary {
	Name = "glslang",
	Pass = "BuildTools",

    Env = {
        CXXOPTS = {
			{ "-fno-strict-aliasing"; Config = { "macosx-*-*", "linux-*-*" } },
        },
    },

    Defines = {
    	"ENABLE_OPT=1",
    },

    Includes = {
		GLSLANG_DIR .. "glslang/Include",
		GLSLANG_DIR .. "glslang",
		GLSLANG_DIR,
		BGFX_DIR .. "3rdparty",
		SPIRV_TOOLS .. "include",
    },

    Sources = {
		get_c_cpp_src(GLSLANG_DIR .. "OGLCompilersDLL"),
		get_c_cpp_src(GLSLANG_DIR .. "StandAlone"),
		get_c_cpp_src(GLSLANG_DIR .. "glslang/GenericCodeGen"),
		get_c_cpp_src(GLSLANG_DIR .. "glslang/MachineIndependent"),
		 get_c_cpp_src(GLSLANG_DIR .. "SPIRV"),
		-- get_c_cpp_src(GLSLANG_DIR .. "glslang/HLSL"),
		get_c_cpp_src(GLSLANG_DIR .. "glslang/CInterface"),
		{ get_c_cpp_src(GLSLANG_DIR .. "glslang/OSDependent/Windows") ; Config = "win64-*-*" },
		{ get_c_cpp_src(GLSLANG_DIR .. "glslang/OSDependent/Unix") ; Config = { "linux-*-*", "macosx-*-*" } },
    }
}

-----------------------------------------------------------------------------------------------------------------------

StaticLibrary {
	Name = "spirv_tools",
	Pass = "BuildTools",

    Env = {
        CXXOPTS = {
			{ "-fno-strict-aliasing"; Config = { "macosx-*-*", "linux-*-*" } },
        },
    },

    Includes = {
		SPIRV_TOOLS,
		SPIRV_TOOLS .. "include",
		SPIRV_TOOLS .. "include/generated",
		SPIRV_HEADERS .. "include",
    },

    Sources = {
		glob_no_main(SPIRV_TOOLS .. "source", ".cpp", false),
		glob_no_main(SPIRV_TOOLS .. "source/opt", ".cpp", false),
		glob_no_main(SPIRV_TOOLS .. "source/util", ".cpp", false),
		glob_no_main(SPIRV_TOOLS .. "source/val", ".cpp", false),
    }
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
				BIMG_DIR .. "include",
				SPIRV_CROSS,
				BX_DIR .. "3rdparty",
				BX_DIR .. "include",
				BGFX_DIR  .. "3rdparty/glslang/glslang/Public",
				BGFX_DIR  .. "3rdparty/glslang/glslang/Include",
				BGFX_DIR  .. "3rdparty/glslang",
				BGFX_DIR  .. "3rdparty/spirv-cross",
				BGFX_DIR  .. "3rdparty/spirv-tools/include",
				BGFX_DIR .. "include",
				BGFX_DIR  .. "3rdparty/webgpu/include",
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
		BX_DIR .. "src/amalgamated.cpp",
		BGFX_DIR .. "tools/shaderc/shaderc.cpp",
		BGFX_DIR .. "tools/shaderc/shaderc_spirv.cpp",
		BGFX_DIR .. "tools/shaderc/shaderc_glsl.cpp",
		BGFX_DIR .. "tools/shaderc/shaderc_hlsl.cpp",
		BGFX_DIR .. "tools/shaderc/shaderc_pssl.cpp",
		BGFX_DIR .. "tools/shaderc/shaderc_metal.cpp",
		BGFX_DIR .. "src/vertexlayout.cpp",
		BGFX_DIR .. "src/vertexlayout.h",
		BGFX_DIR .. "src/shader_spirv.cpp",

		FCPP_DIR .. "cpp1.c",
		FCPP_DIR .. "cpp2.c",
		FCPP_DIR .. "cpp3.c",
		FCPP_DIR .. "cpp4.c",
		FCPP_DIR .. "cpp5.c",
		FCPP_DIR .. "cpp6.c",

		glob_no_main(SPIRV_CROSS, ".cpp", true),
		glob_no_main(GLSL_OPTIMIZER .. "src/mesa", ".c", true),
		glob_no_main(GLSL_OPTIMIZER .. "src/util",  ".c", true),
		glob_c_cpp_no_main(GLSL_OPTIMIZER .. "src/glsl"),
		glob_c_cpp_no_main(GLSL_OPTIMIZER .. "src/glsl/glcpp"),
	},

    Libs = { { "kernel32.lib", "d3dcompiler.lib", "dxguid.lib" ; Config = "win64-*-*" } },

    Frameworks = {
        { "Cocoa" },
        { "Metal" },
        { "QuartzCore" },
        { "OpenGL" }
    },

    Depends = { "glslang", "spirv_tools" },

	IdeGenerationHints = { Msvc = { SolutionFolder = "Tools" } },
}

-----------------------------------------------------------------------------------------

StaticLibrary {
    Name = "glfw",

    Env = {
        CPPPATH = {
            GLFW_DIR .. "src",
            GLFW_DIR .. "include",
        },

        CPPDEFS = {
            { "_GLFW_WIN32", "_GLFW_WGL", "WIN32"; Config = "win64-*-*" },
            { "_GLFW_X11", "_GLFW_GFX", "LINUX"; Config = "linux-*-*" },
            { "_GLFW_COCOA", "MACOSX"; Config = "macosx-*-*" },
        },
    },

    Sources = {
		GLFW_DIR .. "src/window.c",
		GLFW_DIR .. "src/context.c",
		GLFW_DIR .. "src/init.c",
		GLFW_DIR .. "src/input.c",
		GLFW_DIR .. "src/monitor.c",
		GLFW_DIR .. "src/vulkan.c",
        GLFW_DIR .. "src/osmesa_context.c",
        GLFW_DIR .. "src/egl_context.c",

        {
			GLFW_DIR .. "src/cocoa_init.m",
			GLFW_DIR .. "src/cocoa_joystick.m",
			GLFW_DIR .. "src/cocoa_monitor.m",
			GLFW_DIR .. "src/cocoa_time.c",
			GLFW_DIR .. "src/cocoa_window.m",
			GLFW_DIR .. "src/posix_thread.c",
			GLFW_DIR .. "src/nsgl_context.h",
			GLFW_DIR .. "src/nsgl_context.m" ; Config = "macosx-*-*"
		},

		{
			GLFW_DIR .. "src/glx_context.c",
			-- GLFW_DIR .. "src/wl_init.c",
			-- GLFW_DIR .. "src/wl_monitor.c",
			-- GLFW_DIR .. "src/wl_window.c",
			GLFW_DIR .. "src/x11_init.c",
			GLFW_DIR .. "src/x11_monitor.c",
			GLFW_DIR .. "src/x11_window.c",
			GLFW_DIR .. "src/linux_joystick.c",
			GLFW_DIR .. "src/posix_thread.c",
			GLFW_DIR .. "src/posix_time.c",
			GLFW_DIR .. "src/xkb_unicode.c" ; Config = "linux-*-*",
		},

		{
			GLFW_DIR .. "src/wgl_context.c",
			GLFW_DIR .. "src/win32_init.c",
			GLFW_DIR .. "src/win32_joystick.c",
			GLFW_DIR .. "src/win32_monitor.c",
			GLFW_DIR .. "src/win32_thread.c",
			GLFW_DIR .. "src/win32_time.c",
			GLFW_DIR .. "src/win32_window.c" ; Config = "win64-*-*",
		},
    },
}

-----------------------------------------------------------------------------------------

StaticLibrary {
    Name = "bgfx",

    Includes = {
		{
			BX_DIR .. "include/compat/msvc",
			BGFX_DIR .. "3rdparty/dxsdk/include" ; Config = "win64-*-*"
		},

		{
			BX_DIR .. "include/compat/osx" ; Config = "macosx-*-*"
		},

		BGFX_DIR .. "3rdparty/khronos",
		BGFX_DIR .. "3rdparty",
		BGFX_DIR .. "include",
		BX_DIR .. "include",
		BX_DIR .. "3rdparty",
		BIMG_DIR .. "include",
		BIMG_DIR .. "3rdparty",
		BIMG_DIR .. "3rdparty/iqa/include",
		BIMG_DIR .. "3rdparty/astc-codec/include",
	},

    Env = {
        CXXOPTS = {
			{ "-Wno-variadic-macros", "-Wno-everything" ; Config = "macosx-*-*" },
			{ "/EHsc"; Config = "win64-*-*" },
        },
    },

    Defines = {
		"BGFX_CONFIG_RENDERER_WEBGPU=0",
		"BGFX_CONFIG_RENDERER_GNM=0",
		"BGFX_CONFIG_RENDERER_VULKAN=0",
		{ "BGFX_CONFIG_RENDERER_OPENGL=1" ; Config = { "linux-*-*", "win64-*-*" } },
		{ "BGFX_CONFIG_RENDERER_METAL=1" ; Config = "macosx-*-*" },
		{ "BGFX_CONFIG_RENDERER_DIRECT3D11=1"  ; Config = "win64-*-*" },
	},

    Sources = {
    	get_c_cpp_src("src/external/bimg/src"),
		BX_DIR .. "src/amalgamated.cpp",
		BGFX_DIR .. "src/bgfx.cpp",
		BGFX_DIR .. "src/vertexlayout.cpp",
		BGFX_DIR .. "src/debug_renderdoc.cpp",
		BGFX_DIR .. "src/topology.cpp",
		BGFX_DIR .. "src/shader_dxbc.cpp",
		BGFX_DIR .. "src/renderer_gnm.cpp",
		BGFX_DIR .. "src/renderer_webgpu.cpp",
		BGFX_DIR .. "src/renderer_nvn.cpp",
		BGFX_DIR .. "src/renderer_gl.cpp",
		BGFX_DIR .. "src/renderer_vk.cpp",
		BGFX_DIR .. "src/renderer_noop.cpp",
		BGFX_DIR .. "src/renderer_d3d9.cpp",
		BGFX_DIR .. "src/renderer_d3d11.cpp",
		BGFX_DIR .. "src/renderer_d3d12.cpp",
        {
	        BGFX_DIR .. "src/glcontext_nsgl.mm",
            BGFX_DIR .. "src/renderer_mtl.mm" ; Config = "macosx-*-*"
        },
	    {
			BGFX_DIR .. "src/glcontext_wgl.cpp",
			BGFX_DIR .. "src/nvapi.cpp",
			BGFX_DIR .. "src/dxgi.cpp" ; Config = "win64-*-*"
		},
	    {
	        BGFX_DIR .. "src/glcontext_glx.cpp" ; Config = "linux-*-*"
	    },
    },

	IdeGenerationHints = { Msvc = { SolutionFolder = "External" } },
}

Default "bgfx_shaderc"
