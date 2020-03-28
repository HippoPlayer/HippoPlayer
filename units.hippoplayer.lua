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
        "src/external/toolwindowmanager/src/ToolWindowManager.cpp",
        "src/external/toolwindowmanager/src/ToolWindowManagerArea.cpp",
        "src/external/toolwindowmanager/src/ToolWindowManagerSplitter.cpp",
        "src/external/toolwindowmanager/src/ToolWindowManagerTabBar.cpp",
        "src/external/toolwindowmanager/src/ToolWindowManagerWrapper.cpp",

        gen_moc("src/external/toolwindowmanager/src/ToolWindowManager.h"),
        gen_moc("src/external/toolwindowmanager/src/ToolWindowManagerArea.h"),
        gen_moc("src/external/toolwindowmanager/src/ToolWindowManagerSplitter.h"),
        gen_moc("src/external/toolwindowmanager/src/ToolWindowManagerTabBar.h"),
        gen_moc("src/external/toolwindowmanager/src/ToolWindowManagerWrapper.h"),

    --[[
        "src/external/qt_advanced_docking_system/src/ads_globals.cpp",
        "src/external/qt_advanced_docking_system/src/DockAreaWidget.cpp",
        "src/external/qt_advanced_docking_system/src/DockAreaTabBar.cpp",
        "src/external/qt_advanced_docking_system/src/DockContainerWidget.cpp",
        "src/external/qt_advanced_docking_system/src/DockManager.cpp",
        "src/external/qt_advanced_docking_system/src/DockWidget.cpp",
        "src/external/qt_advanced_docking_system/src/DockWidgetTab.cpp",
        "src/external/qt_advanced_docking_system/src/FloatingDockContainer.cpp",
        "src/external/qt_advanced_docking_system/src/DockOverlay.cpp",
        "src/external/qt_advanced_docking_system/src/DockSplitter.cpp",
        "src/external/qt_advanced_docking_system/src/DockAreaTitleBar.cpp",
        "src/external/qt_advanced_docking_system/src/ElidingLabel.cpp",
        "src/external/qt_advanced_docking_system/src/linux/FloatingWidgetTitleBar.cpp",

        gen_moc("src/external/qt_advanced_docking_system/src/linux/FloatingWidgetTitleBar.h"),

        gen_moc("src/external/qt_advanced_docking_system/src/DockAreaTabBar.h"),
        gen_moc("src/external/qt_advanced_docking_system/src/DockAreaTitleBar.h"),
        gen_moc("src/external/qt_advanced_docking_system/src/DockAreaWidget.h"),
        gen_moc("src/external/qt_advanced_docking_system/src/DockContainerWidget.h"),
        gen_moc("src/external/qt_advanced_docking_system/src/DockManager.h"),
        gen_moc("src/external/qt_advanced_docking_system/src/DockOverlay.h"),
        gen_moc("src/external/qt_advanced_docking_system/src/DockSplitter.h"),
        gen_moc("src/external/qt_advanced_docking_system/src/DockWidget.h"),
        gen_moc("src/external/qt_advanced_docking_system/src/DockWidgetTab.h"),
        gen_moc("src/external/qt_advanced_docking_system/src/ElidingLabel.h"),
        gen_moc("src/external/qt_advanced_docking_system/src/FloatingDockContainer.h"),
        gen_rcc("src/external/qt_advanced_docking_system/src/ads.qrc"),
    --]]
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
        get_rs_src("src/hippo_core"),
        "src/hippo_core/ffi/src/lib.rs",
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

        gen_moc("src/hippo_player/MainWindow.h"),
        gen_moc("src/hippo_player/PlaylistModel.h"),
        --gen_uic("src/hippo_player/test_main/MainWindow.ui"),
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
            -- "QT_NO_KEYWORDS",
            "QT_NO_CAST_FROM_ASCII",
            "QT_NO_CAST_TO_ASCII",
        },

        CPPPATH = {
            "src/external/qt_advanced_docking_system/src",
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
        },
    },

	Libs = {
		{ "wsock32.lib", "kernel32.lib", "user32.lib", "gdi32.lib", "Comdlg32.lib",
		  "Advapi32.lib", "Qt5Guid.lib", "Qt5Cored.lib", "Qt5Widgetsd.lib"; Config = "win64-*-*" },
		{ "Qt5Widgets", "Qt5Gui", "Qt5Core"; Config = "linux-*-*" },
	},

    Frameworks = { "Cocoa", "QtWidgets", "QtGui", "QtCore" },

    Depends = { "hippo_core", "hippo_core_loader", "qt_addons", "flatbuffers_lib" },
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

-- vim: ts=4:sw=4:sts=4

