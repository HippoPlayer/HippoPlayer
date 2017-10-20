require "tundra.syntax.glob"
require "tundra.path"
require "tundra.util"
require "tundra.syntax.rust-cargo"

local native = require('tundra.native')

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
		Extensions = { ".cpp", ".c" },
		Recursive = true,
	}
end

-----------------------------------------------------------------------------------------------------------------------

SharedLibrary {
	Name = "HivelyPlugin",
	Sources = {
		"src/plugins/hively/HivelyPlugin.c",
		"src/plugins/hively/replayer/hvl_replay.c"
	},
}

-----------------------------------------------------------------------------------------------------------------------

SharedLibrary {
	Name = "OpenMPT",
	Defines = { "LIBOPENMPT_BUILD" },
	Includes = {
		"src/plugins/openmpt/libopenmpt",
		"src/plugins/openmpt/libopenmpt/common"
	},

	Sources = {
		get_c_cpp_src("src/plugins/openmpt/libopenmpt/soundlib"),
		get_c_cpp_src("src/plugins/openmpt/libopenmpt/common"),
		get_c_cpp_src("src/plugins/openmpt/libopenmpt/sounddsp"),
		"src/plugins/openmpt/libopenmpt/libopenmpt/libopenmpt_c.cpp",
		"src/plugins/openmpt/libopenmpt/libopenmpt/libopenmpt_cxx.cpp",
		"src/plugins/openmpt/libopenmpt/libopenmpt/libopenmpt_impl.cpp",
		"src/plugins/openmpt/libopenmpt/libopenmpt/libopenmpt_ext_impl.cpp",
		"src/plugins/openmpt/OpenMPT.cpp",
	},

	Libs = { "stdc++" },
}

-----------------------------------------------------------------------------------------------------------------------

SharedLibrary {
	Name = "SidPlugin",
	Includes = {
		"src/plugins/sid/libsidplayfp/src",
	},

	Sources = {
		get_c_cpp_src("src/plugins/sid"),
	},

	Libs = { "stdc++" },
}

-----------------------------------------------------------------------------------------------------------------------

SharedLibrary {
	Name = "FutureComposerPlugin",
	Env = {
		CPPPATH = { "src/plugins/futurecomposer/src" },
		CPPDEFS = {
			{ "HAVE_CONFIG_H"; Config = "macosx-*-*" }
		},
	},

	Sources = {
		get_c_cpp_src("src/plugins/futurecomposer/src"),
		"src/plugins/futurecomposer/FutureComposerPlugin.c",
	},

	Libs = {
		{ "stdc++"; Config = "macosx-*-*" },
	},
}

-----------------------------------------------------------------------------------------------------------------------

SharedLibrary {
	Name = "TfmxPlugin",

	Sources = {
		get_c_cpp_src("src/plugins/tfmx/src"),
	},
}


-----------------------------------------------------------------------------------------------------------------------
--[[
RustCrate {
	Name = "dummy",
	CargoConfig = "api/rust/prodbg/Cargo.toml",
	Sources = {
		get_rs_src("api/rust/dummy"),
	},
}
-----------------------------------------------------------------------------------------------------------------------

RustSharedLibrary {
	Name = "amiga_uae_plugin",
	CargoConfig = "src/addons/amiga_uae_plugin/Cargo.toml",
	Sources = {
		get_rs_src("src/addons/amiga_uae_plugin"),
		get_rs_src("src/crates/amiga_hunk_parser"),
		get_rs_src("src/crates/gdb-remote"),
		get_rs_src("api/rust/prodbg"),
	}
}
--]]

--Default "amiga_uae_plugin"
--Default "memory_view_2"
--Default "dummy_backend_plugin"

-- vim: ts=4:sw=4:sts=4

Default "HivelyPlugin"
Default "OpenMPT"
Default "FutureComposerPlugin"
Default "TfmxPlugin"
Default "SidPlugin"

