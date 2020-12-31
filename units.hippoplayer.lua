require "tundra.syntax.glob"
require "tundra.syntax.qt"
require "tundra.syntax.rust-cargo"
require "tundra.syntax.osx-bundle"
require "tundra.path"
require "tundra.util"

local native = require('tundra.native')

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

Program {
    Name = "hippo_vis",

	Includes = {
		"src/external/bx/include",
		"src/external/bgfx/include",
		"src/external/glfw/include",
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

