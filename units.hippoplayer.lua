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

SharedLibrary {
    Name = "wrui_qt",
    Sources = {
        Glob {
            Dir = "src/external/wrui_qt",
            Extensions = { ".cpp", ".h" },
            Recursive = true,
        },

        gen_moc("src/external/wrui_qt/qt_api_gen.h"),
        gen_moc("src/external/wrui_qt/ToolWindowManager/ToolWindowManager.h"),
        gen_moc("src/external/wrui_qt/ToolWindowManager/ToolWindowManagerArea.h"),
        gen_moc("src/external/wrui_qt/ToolWindowManager/ToolWindowManagerSplitter.h"),
        gen_moc("src/external/wrui_qt/ToolWindowManager/ToolWindowManagerTabBar.h"),
        gen_moc("src/external/wrui_qt/ToolWindowManager/ToolWindowManagerWrapper.h"),

        gen_moc("src/external/wrui_qt/FramlessWindow/framelesswindow/framelesswindow.h"),
        gen_moc("src/external/wrui_qt/FramlessWindow/framelesswindow/windowdragger.h"),
        gen_moc("src/external/wrui_qt/FramlessWindow/DarkStyle.h"),

        gen_uic("src/external/wrui_qt/FramlessWindow/framelesswindow/framelesswindow.ui"),
        gen_rcc("src/external/wrui_qt/FramlessWindow/darkstyle.qrc"),
    },

    Env = {
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

        CPPDEFS = {
            "QT_NO_CAST_FROM_ASCII",
            "QT_NO_CAST_TO_ASCII",
        },

        CPPPATH = {
            "src/external/wrui_qt/ToolWindowManager/",
            "src/external/wrui_qt/ToolWindowManager/",
			"src/external/wrui_qt/FramlessWindow/framelesswindow",
			"src/external/wrui_qt/FramlessWindow",

            "$(QT5)/include",
            "$(QT5)/include/QtCore",
            "$(QT5)/include/QtGui",
            "$(QT5)/include/QtWidgets",

            "$(OBJECTROOT)", "$(OBJECTDIR)",
        },

        LIBPATH = {
			{ "$(QT5)\\lib"; Config = "win64-*-*" },
			{ "$(QT5)/lib"; Config = "linux-*-*" },
		},

        SHLIBOPTS = {
            {  "-Wl,-rpath,$(QT5)/lib", "-F$(QT5)/lib", "-lstdc++", Config = "macosx-clang-*" },
            {  "-Wl,-rpath,$(QT5)/lib", "-lstdc++", "-lm", Config = "linux-*-*" },
        },
    },

	Libs = {
		{ "wsock32.lib", "kernel32.lib", "user32.lib", "gdi32.lib", "Comdlg32.lib",
		  "Advapi32.lib", "Qt5Gui.lib", "Qt5Core.lib", "Qt5Widgets.lib"; Config = "win64-*-*" },
		{ "Qt5Gui", "Qt5Core", "Qt5Widgets"; Config = "linux-*-*" },
	},

    Frameworks = { "Cocoa", "QtWidgets", "QtGui", "QtCore" },
}

-----------------------------------------------------------------------------------------------------------------------

local function get_rs_src(dir)
    return Glob {
        Dir = dir,
        Extensions = { ".rs" },
        Recursive = true,
}
end

-----------------------------------------------------------------------------------------------------------------------

RustCrate {
    Name = "wrui_rust",
    CargoConfig = "src/external/wrui_rust/Cargo.toml",
    Sources = {
        get_rs_src("src/external/wrui_rust"),
    },
}

-----------------------------------------------------------------------------------------------------------------------

RustProgram {
    Name = "hippo_player",
    CargoConfig = "src/hippo_player/Cargo.toml",
    Sources = {
        get_rs_src("src/hippo_player/src"),
		get_rs_src("src/plugin_api/rust/hippo_api"),
    },
    Depends = { "wrui_rust", "wrui_qt" },
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

