package = "moonadditions"
version = "1.1.0-1"
source = {
   url = "git+https://github.com/THE-FYP/MoonAdditions.git",
   tag = "v1.1.0"
}
description = {
   summary = "A bunch of useful things for MoonLoader",
   detailed = "MoonAdditions is a Lua module for MoonLoader that adds bunch of useful scripting functions and gives new capabilities for mods developers.",
   homepage = "https://github.com/THE-FYP/MoonAdditions",
   license = "MIT"
}
dependencies = {
   "lua ~> 5.1"
}
supported_platforms = {"windows"}
external_dependencies = {
   PLUGIN_SDK = {
      header = "plugin.h"
   }
}
build = {
   type = "builtin",
   modules = {
      MoonAdditions = {
         sources = {
            "src/vehicle_renderer.cpp",
            "src/additional_events.cpp",
            "src/lua_bone.cpp",
            "src/lua_component.cpp",
            "src/lua_general.cpp",
            "src/lua_matrix.cpp",
            "src/lua_quat.cpp",
            "src/lua_shape.cpp",
            "src/lua_textdraw.cpp",
            "src/lua_texture.cpp",
            "src/lua_txd.cpp",
            "src/lua_vector.cpp",
            "src/main.cpp",
            "src/pch.cpp",
            "src/shape_drawer.cpp",
            "src/spotlight_drawer.cpp",
            "src/sprite_drawer.cpp",
            "src/text_drawer.cpp",
            "src/texture_manager.cpp",
         },
         defines = {
            "GTASA",
            "PLUGIN_SGV_10US",
            "SOL_EXCEPTIONS_SAFE_PROPAGATION",
            "SOL_CHECK_ARGUMENTS",
            "SOL_NO_CHECK_NUMBER_PRECISION",
            "SOL_STRINGS_ARE_NUMBERS"
         },
         incdirs = {
            "lib/sol2",
            "$(PLUGIN_SDK_DIR)/plugin_sa",
            "$(PLUGIN_SDK_DIR)/plugin_sa/game_sa",
            "$(PLUGIN_SDK_DIR)/shared",
            "$(PLUGIN_SDK_DIR)/shared/game"
         },
		 libdirs = {"$(PLUGIN_SDK_DIR)/output/lib"},
		 libraries = {"plugin"}
      }
   }
}
