require "tundra.syntax.glob"
require "tundra.syntax.rust-cargo"
require "tundra.syntax.osx-bundle"
require "tundra.path"
require "tundra.util"

local native = require('tundra.native')

-----------------------------------------------------------------------------------------------------------------------
--[[
DefRule {
    Name = "GenWruiFFIBindings",
    Pass = "CodeGeneration",

    ConfigInvariant = true,

    Command = "bindgen $(<) -o $(@)",

    Blueprint = {
        Input = { Type = "string", Required = true },
        Output = { Type = "string", Required = true },
    },

    Setup = function (env, data)
        return {
            InputFiles = { data.Input },
            OutputFiles = { "$(OBJECTROOT)$(SEP)_generated$(SEP)" .. data.Output },
        }
    end,
}
--]]

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
    CargoConfig = "src/wrui_rust/Cargo.toml",
    Sources = {
        "src/wrui/include/wrui.h",
        get_rs_src("src/wrui_rust"),
    },
}


-----------------------------------------------------------------------------------------------------------------------

RustProgram {
    Name = "hippo_player",
    CargoConfig = "src/hippo_player/Cargo.toml",
    Sources = get_rs_src("src/hippo_player/src"),
    Depends = { "wrui_rust" },
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
        "data/mac/icon.icns",
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

