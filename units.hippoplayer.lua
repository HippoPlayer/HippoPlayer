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
        Glob {
            Dir = "src/external/rute_cpp",
            Extensions = { ".cpp", ".h" },
            Recursive = true,
        },

        gen_moc("src/external/rute_cpp/ToolWindowManager/ToolWindowManager.h"),
        gen_moc("src/external/rute_cpp/ToolWindowManager/ToolWindowManagerArea.h"),
        gen_moc("src/external/rute_cpp/ToolWindowManager/ToolWindowManagerSplitter.h"),
        gen_moc("src/external/rute_cpp/ToolWindowManager/ToolWindowManagerTabBar.h"),
        gen_moc("src/external/rute_cpp/ToolWindowManager/ToolWindowManagerWrapper.h"),

        gen_moc("src/external/rute_cpp/AdvancedDockingSystem/DockContainerWidget.h"),
        gen_moc("src/external/rute_cpp/AdvancedDockingSystem/DockManager.h"),
        gen_moc("src/external/rute_cpp/AdvancedDockingSystem/DockOverlay.h"),
        gen_moc("src/external/rute_cpp/AdvancedDockingSystem/DockSplitter.h"),
        gen_moc("src/external/rute_cpp/AdvancedDockingSystem/DockStateSerialization.h"),
        gen_moc("src/external/rute_cpp/AdvancedDockingSystem/DockAreaWidget.h"),
        gen_moc("src/external/rute_cpp/AdvancedDockingSystem/DockWidget.h"),
        gen_moc("src/external/rute_cpp/AdvancedDockingSystem/DockWidgetTitleBar.h"),
        gen_moc("src/external/rute_cpp/AdvancedDockingSystem/FloatingDockContainer.h"),
        gen_moc("src/external/rute_cpp/AdvancedDockingSystem/ads_globals.h"),

        gen_moc("src/external/rute_cpp/FramlessWindow/framelesswindow/framelesswindow.h"),
        gen_moc("src/external/rute_cpp/FramlessWindow/framelesswindow/windowdragger.h"),
        gen_moc("src/external/rute_cpp/FramlessWindow/DarkStyle.h"),

        gen_uic("src/external/rute_cpp/FramlessWindow/framelesswindow/framelesswindow.ui"),
        gen_rcc("src/external/rute_cpp/FramlessWindow/darkstyle.qrc"),
        -- gen_rcc("src/external/rute_cpp/AdvancedDockingSystem/ads.qrc"),
    },

    Env = {
       --[[
       CXXOPTS = {
            { "-isystem $(QT5)/lib/QtWidgets.framework/Headers",
              "-isystem $(QT5)/lib/QtCore.framework/Headers",
              "-isystem $(QT5)/lib/QtGui.framework/Headers",
              "-F$(QT5)/lib"; Config = "macosx-*-*" },

            { "-isystem $(QT5)/include/QtWidgets",
              "-isystem $(QT5)/include/QtCore",
              "-isystem $(QT5)/include/QtGui",
              "-isystem $(QT5)/include"; Config = "linux-*-*" },
        },
        --]]

        CPPPATH = {
            "src/external/rute_cpp/ToolWindowManager/",
            "src/external/rute_cpp/ToolWindowManager/",
			"src/external/rute_cpp/FramlessWindow/framelesswindow",
			"src/external/rute_cpp/FramlessWindow",
			"src/external/rute_cpp/AdvancedDockingSystem",

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
    CargoConfig = "src/hippo_core/Cargo.toml",
    Sources = {
        get_rs_src("src/hippo_core/src"),
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
    },

    Env = {
       CXXOPTS = {
            { "-isystem $(QT5_LIB)/QtWidgets.framework/Headers",
              "-isystem $(QT5_LIB)/QtCore.framework/Headers",
              "-isystem $(QT5_LIB)/QtGui.framework/Headers",
              "-F$(QT5_LIB)/lib"; Config = "macosx-*-*" },

            { "-isystem $(QT5_INC)"; Config = "linux-*-*" },
        },

        CPPDEFS = {
            "QT_NO_KEYWORDS",
            "QT_NO_CAST_FROM_ASCII",
            "QT_NO_CAST_TO_ASCII",
        },

        CPPPATH = {
            "$(QT5_INC)",
            "$(OBJECTROOT)", "$(OBJECTDIR)",
        },

        LIBPATH = {
			{ "$(QT5_LIB)"; Config = "win64-*-*" },
			{ "$(QT5_LIB)"; Config = "linux-*-*" },
		},

        PROGCOM = {
            {  "-Wl,-rpath,$(QT5_LIB)/lib", "-F$(QT5_LIB)/lib", "-lstdc++", Config = "macosx-clang-*" },
            {  "-L$(QT5_LIB)", "-lstdc++", "-lm", "-ldl", Config = "linux-*-*" },
        },
    },

	Libs = {
		{ "wsock32.lib", "kernel32.lib", "user32.lib", "gdi32.lib", "Comdlg32.lib",
		  "Advapi32.lib", "Qt5Gui.lib", "Qt5Core.lib", "Qt5Widgets.lib"; Config = "win64-*-*" },
		{ "Qt5Gui", "Qt5Core", "Qt5Widgets"; Config = "linux-*-*" },
	},

    Frameworks = { "Cocoa", "QtWidgets", "QtGui", "QtCore" },

    Depends = { "hippo_core", "hippo_core_loader", "qt_addons" },
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

