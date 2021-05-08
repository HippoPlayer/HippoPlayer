require "tundra.syntax.glob"
require "tundra.syntax.qt"
require "tundra.syntax.rust-cargo"
require "tundra.syntax.osx-bundle"
require "tundra.path"
require "tundra.util"

local native = require('tundra.native')

-----------------------------------------------------------------------------------------------------------------------

local GLFW_DIR = "src/external/glfw/"
local BIMG_DIR = "src/external/bimg/"
local BX_DIR = "src/external/bx/"
local STB_DIR = "src/external/stb/"

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

DefRule {
    Name = "ShadercFS",
    Command = "$(BGFX_SHADERC) -i external/bgfx/src -f $(<) -o $(@) --type fragment --platform " .. shaderc_platform .. shaderc_ps_extra_params,

    Blueprint = {
        Source = { Required = true, Type = "string", Help = "Input filename", },
        OutName = { Required = false, Type = "string", Help = "Output filename", },
    },

    Setup = function (env, data)
        return {
            InputFiles    = { data.Source },
            OutputFiles   = { "$(OBJECTDIR)/_generated/" .. tundra.path.drop_suffix(data.Source) .. ".fs" },
        }
    end,
}

-----------------------------------------------------------------------------------------------------------------------

DefRule {
    Name = "ShadercVS",
    Command = "$(BGFX_SHADERC) -i external/bgfx/src -f $(<) -o $(@) --type vertex --platform " .. shaderc_platform .. shaderc_vs_extra_params,

    Blueprint = {
        Source = { Required = true, Type = "string", Help = "Input filename", },
        OutName = { Required = false, Type = "string", Help = "Output filename", },
    },

    Setup = function (env, data)
        return {
            InputFiles    = { data.Source },
            OutputFiles   = { "$(OBJECTDIR)/_generated/" .. tundra.path.drop_suffix(data.Source) .. ".vs" },
        }
    end,
}

-----------------------------------------------------------------------------------------------------------------------

local function gen_moc(src)
    return Moc {
        Pass = "GenerateSources",
        Source = src
    }
end

-----------------------------------------------------------------------------------------------------------------------

local function gen_uic(src)
    return Uic {
        Pass = "GenerateSources",
        Source = src
    }
end

-----------------------------------------------------------------------------------------------------------------------

local function gen_rcc(src)
    return Rcc {
        Pass = "GenerateSources",
        Source = src
    }
end

-----------------------------------------------------------------------------------------------------------------------

local function get_rs_src(dir)
    return Glob {
        Dir = dir,
        Extensions = { ".rs" },
        Recursive = true,
}
end

-----------------------------------------------------------------------------------------------------------------------

local function get_c_cpp_src(dir)
    return Glob {
        Dir = dir,
        Extensions = { ".cpp", ".c", ".h" },
        Recursive = true,
}
end

-----------------------------------------------------------------------------------------------------------------------

StaticLibrary {
    Name = "qt_addons",
    Sources = {
        "src/external/fastdock/FastDock.cpp",
        "src/external/fastdock/FastDockArea.cpp",
        "src/external/fastdock/FastDockSplitter.cpp",
        "src/external/fastdock/FastDockTabBar.cpp",
        "src/external/fastdock/FastDockWrapper.cpp",

        gen_moc("src/external/fastdock/FastDock.h"),
        gen_moc("src/external/fastdock/FastDockArea.h"),
        gen_moc("src/external/fastdock/FastDockSplitter.h"),
        gen_moc("src/external/fastdock/FastDockTabBar.h"),
        gen_moc("src/external/fastdock/FastDockWrapper.h"),
    },

    Env = {
       CXXOPTS = {
            { "-isystem $(QT5_LIB)/QtWidgets.framework/Headers",
              "-isystem $(QT5_LIB)/QtCore.framework/Headers",
              "-isystem $(QT5_LIB)/QtGui.framework/Headers",
              "-F$(QT5_LIB)"; Config = "macosx-*-*" },
            -- { "-DADS_STATIC -DQT_CORE_LIB -DQT_GUI_LIB -DQT_NO_DEBUG -DQT_WIDGETS_LIB -fPIC -std=c++14", Config = "linux-*-*" },
        },

        CPPPATH = {
            -- "src/external/qt_advanced_docking_system/src",
            "$(QT5_INC)",
            "$(QT5_INC)/QtCore",
            "$(QT5_INC)/QtGui",
            "$(QT5_INC)/QtWidgets",
            "$(OBJECTROOT)", "$(OBJECTDIR)",
        },
    }
}

-----------------------------------------------------------------------------------------------------------------------

RustSharedLibrary {
    Name = "hippo_core",
    CargoConfig = "src/hippo_core/core/Cargo.toml",
    Sources = {
        "src/hippo_core/miniaudio/src/c_api/miniaudio.h",
        get_rs_src("src/hippo_core"),
        "src/plugin_api/HippoPlugin.h",
    },
}

-----------------------------------------------------------------------------------------------------------------------

StaticLibrary {
    Name = "hippo_core_loader",
    Sources = {
        get_c_cpp_src("src/hippo_core_loader"),
    },
}

-----------------------------------------------------------------------------------------------------------------------

Program {
    Name = "hippo_player",

    Sources = {
        Glob {
            Dir = "src/hippo_player",
            Extensions = { ".cpp" },
            Recursive = true,
        },

        gen_moc("src/hippo_player/console.h"),
        gen_uic("src/hippo_player/console.ui"),

        gen_moc("src/hippo_player/main_window.h"),
        gen_moc("src/hippo_player/playlist_model.h"),

        -- Dialogs
        gen_uic("src/hippo_player/prefs_dialog.ui"),
        gen_moc("src/hippo_player/prefs_dialog.h"),
        gen_uic("src/hippo_player/playback_plugin_panel.ui"),
        gen_moc("src/hippo_player/playback_plugin_panel.h"),
        gen_uic("src/hippo_player/playback_plugin_priority.ui"),
        gen_moc("src/hippo_player/playback_plugin_priority.h"),
        gen_uic("src/hippo_player/device_panel.ui"),
        gen_moc("src/hippo_player/device_panel.h"),
    },

    Env = {
       CXXOPTS = {
            { "-isystem $(QT5_LIB)/QtWidgets.framework/Headers",
              "-isystem $(QT5_LIB)/QtCore.framework/Headers",
              "-isystem $(QT5_LIB)/QtGui.framework/Headers",
              "-F$(QT5_LIB)"; Config = "macosx-*-*" },
            -- { "-DQT_CORE_LIB -DQT_GUI_LIB -DQT_NO_DEBUG -DQT_WIDGETS_LIB", "-isystem $(QT5_INC)"; Config = "linux-*-*" },
        },

        CPPDEFS = {
            "QT_NO_KEYWORDS",
            "QT_NO_CAST_FROM_ASCII",
            "QT_NO_CAST_TO_ASCII",
        },

        CPPPATH = {
            "$(QT5_INC)",
            "$(QT5_INC)/QtCore",
            "$(QT5_INC)/QtWidgets",
            "$(QT5_INC)/QtGui",
            "$(OBJECTROOT)", "$(OBJECTDIR)",
        },

        LIBPATH = {
			{ "$(QT5_LIB)"; Config = "win64-*-*" },
			{ "$(QT5_LIB)"; Config = "linux-*-*" },
		},

        PROGCOM = {
            {  "-Wl,-rpath,$(QT5_LIB)", "-F$(QT5_LIB)", "-lstdc++", Config = "macosx-clang-*" },
            {  "-Wl,-rpath,$(QT5_LIB)", "-ldl"; Config = "linux-*-*" },
            { "/SUBSYSTEM:WINDOWS" ; Config = "win64-*-*" },
        },
    },

	Libs = {
        { "data/windows/hippo.res", "Qt5Gui.lib", "Qt5Core.lib", "Qt5Widgets.lib", "qtmain.lib", "shell32.lib"; Config = "win64-*-*" },
		{ "Qt5Widgets", "Qt5Gui", "Qt5Core"; Config = "linux-*-*" },
	},

    Frameworks = { "Cocoa", "QtWidgets", "QtGui", "QtCore" },

    Depends = { "hippo_core", "hippo_core_loader", "qt_addons", "flatbuffers_lib" },
}

-----------------------------------------------------------------------------------------------------------------------

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
            { "_GLFW_COCOA", "MACOSX"; Config = "macos-*-*" },
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
            GLFW_DIR .. "src/nsgl_context.m" ; Config = "macos-*-*"
        },

        {
            GLFW_DIR .. "src/glx_context.c",
            -- GLFW_DIR .. "src/wl_init.c",
            --GLFW_DIR .. "src/wl_monitor.c",
            --GLFW_DIR .. "src/wl_window.c",
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


-----------------------------------------------------------------------------------------------------------------------

Program {
    Name = "hippo_vis",

	Includes = {
        BIMG_DIR .. "include",
		"src/external/bx/include",
		"src/external/bgfx/include",
		"src/external/glfw/include",
		"src/external",
        { BX_DIR .. "/include/compat/msvc" ; Config = "win64-*-*" },
	},

	Defines = {
		"BX_CONFIG_DEBUG=1",
		{ "GLFW_EXPOSE_NATIVE_WIN32" ; Config = "win64-*-*" },
		{ "GLFW_EXPOSE_NATIVE_COCOA" ; Config = "macos*-*-*" },
		{ "GLFW_EXPOSE_NATIVE_X11" ; Config = "linux-*-*" },
	},

    Sources = {
        Glob {
            Dir = "src/hippo_vis",
            Extensions = { ".cpp" },
            Recursive = true,
        },
    },

    Env = {
		PROGCOM = {
            { "data/windows/hippo.res", "opengl32.lib", "shell32.lib", "qtmain.lib", "gdi32.lib", "user32.lib"; Config = "win64-*-*" },
			{  "-lGL", "-lX11", "-lpthread", "-ldl"; Config = "linux-*-*" },
			{  "-lc++"; Config = "macosx-*-*" },
		},
	},

    Frameworks = { "Cocoa", "IOKit", "Metal", "QuartzCore", "MetalKit" },

    Depends = { "bgfx", "glfw", "hippo_core_loader", "flatbuffers_lib" },
}

-----------------------------------------------------------------------------------------------------------------------

local hippoBundle = OsxBundle
{
    Depends = { "hippo_player" },
    Target = "$(OBJECTDIR)/HippoPlayer.app",
    InfoPList = "data/mac/Info.plist",
    Executable = "$(OBJECTDIR)/hippo_player",
    Resources = {
        CompileNib { Source = "data/mac/appnib.xib", Target = "appnib.nib" },
        "data/mac/hippo.icns",
    },

    Config = { "macosx-clang-debug-default" ; "macosx-clang-release-default" },
}

-----------------------------------------------------------------------------------------------------------------------

if native.host_platform == "macosx" then
    Default "hippo_player"
    Default(hippoBundle)
else
    Default "hippo_player"
end

Default "hippo_vis"

-- vim: ts=4:sw=4:sts=4

