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

SharedLibrary {
	Name = "HivelyPlugin",
	Sources = {
		"old_src/plugins/hively/HivelyPlugin.c",
		"old_src/plugins/hively/replayer/hvl_replay.c"
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

