local native = require('tundra.native')

-----------------------------------------------------------------------------------------------------------------------

local mac_opts = {
    "-Wall", "-I.",
    -- "-Weverything", "-Werror",
    { "-O0", "-g"; Config = "*-*-debug" },
    { "-O3", "-g"; Config = "*-*-release" },
}

-----------------------------------------------------------------------------------------------------------------------

local macosx = {
    Env = {
        RUST_CARGO_OPTS = {
            { "test"; Config = "*-*-*-test" },
        },

        CCOPTS =  {
            mac_opts,
        },

        CXXOPTS = {
            mac_opts,
            "-std=c++11",
        },

        SHLIBOPTS = { "-lstdc++" },
        PROGCOM = { "-lstdc++" },
    },

    Frameworks = {
        { "Cocoa" },
        { "Metal" },
        { "QuartzCore" },
        { "OpenGL" }
    },
}

-----------------------------------------------------------------------------------------------------------------------

local gcc_opts = {
    "-I.",
    "-Wno-array-bounds", "-Wno-attributes", "-Wno-unused-value",
    "-DOBJECT_DIR=\\\"$(OBJECTDIR)\\\"",
    "-Wall",
    "-fPIC",
    { "-O0", "-g"; Config = "*-*-debug" },
    { "-O3", "-g"; Config = "*-*-release" },
}

local gcc_env = {
    Env = {
        RUST_CARGO_OPTS = {
            { "test"; Config = "*-*-*-test" },
        },

        CCOPTS = {
            gcc_opts,
        },

        CXXOPTS = {
            gcc_opts,
            "-std=c++11",
        },

    },
}

-----------------------------------------------------------------------------------------------------------------------

local win64_opts = {
    "/EHsc", "/FS", "/MT", "/W3", "/I.", "/DUNICODE", "/D_UNICODE", "/DWIN32", "/D_CRT_SECURE_NO_WARNINGS",
    "\"/DOBJECT_DIR=$(OBJECTDIR:#)\"",
    { "/Od"; Config = "*-*-debug" },
    { "/O2"; Config = "*-*-release" },
}

local win64 = {
    Env = {
        RUST_CARGO_OPTS = {
            { "test"; Config = "*-*-*-test" },
        },

        GENERATE_PDB = "1",
        CCOPTS = {
            win64_opts,
        },

        CXXOPTS = {
            win64_opts,
        },

        OBJCCOM = "meh",
    },
}

-----------------------------------------------------------------------------------------------------------------------

Build {
    Passes = {
        BuildTools = { Name="Build Tools", BuildOrder = 1 },
        GenerateSources = { Name="Generate sources", BuildOrder = 2 },
    },

    Units = {
        "units.hippoplayer.lua",
        "units.plugins.lua",
    },

    Configs = {
        Config { Name = "macosx-clang", DefaultOnHost = "macosx", Inherit = macosx, Tools = { "clang-osx", "rust", "qt" } },
        Config { Name = "win64-msvc", DefaultOnHost = { "windows" }, Inherit = win64, Tools = { "msvc", "rust", "qt" } },
        Config { Name = "linux-gcc", DefaultOnHost = { "linux" }, Inherit = gcc_env, Tools = { "gcc", "rust", "qt" } },
    },

    IdeGenerationHints = {
        Msvc = {
            -- Remap config names to MSVC platform names (affects things like header scanning & debugging)
            PlatformMappings = {
                ['win64-msvc'] = 'x64',
            },
            -- Remap variant names to MSVC friendly names
            VariantMappings = {
                ['release']    = 'Release',
                ['debug']      = 'Debug',
            },
        },

        MsvcSolutions = {
            ['HippoPlayer.sln'] = { }
        },

    },

    Variants = { "debug", "release" },
    SubVariants = { "default", "test" },
}

-- vim: ts=4:sw=4:sts=4

