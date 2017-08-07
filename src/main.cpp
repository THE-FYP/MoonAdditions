// MIT License

// Copyright (c) 2012 DK22Pac
// Copyright (c) 2017 FYP

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "pch.h"
#include "lua_module.h"
#include "lua_texture.h"
#include "lua_vector.h"
#include "lua_quat.h"
#include "lua_matrix.h"
#include "lua_bone.h"
#include "lua_textdraw.h"
#include "lua_txd.h"
#include "lua_component.h"
#include "lua_general.h"
#include "lua_shape.h"
#include "sprite_drawer.h"
#include "text_drawer.h"
#include "spotlight_drawer.h"
#include "shape_drawer.h"
#include "vehicle_renderer.h"
#include "additional_events.h"


static void initializeEvents()
{
	static bool initialized = false;
	if (initialized)
		return;
	initialized = true;

	plugin::Events::drawScriptSpotlightsEvent += [] {
		SpotlightDrawer::get()->draw();
	};

	plugin::Events::drawingEvent += [] {
		SpriteDrawer::get()->draw();
		ShapeDrawer::get()->drawShapes();
		TextDrawer::get()->drawTextdraws();
		TextDrawer::get()->draw();
	};

	plugin::Events::vehicleRenderEvent.before += [] (CVehicle* veh) {
		if (veh)
			VehicleRenderer::get()->processRender(veh);
	};

	plugin::Events::vehicleResetAfterRender += [](CVehicle* veh) {
		if (veh)
			VehicleRenderer::get()->postRender(veh);
	};
}

sol::table open(sol::this_state ts)
{
	sol::state_view lua(ts);
	sol::table module = lua.create_table();
	module["VERSION"] = "1.0.0";

	lua_vector::inizialize(module);
	lua_quat::initialize(module);
	lua_matrix::initialize(module);
	lua_texture::initialize(module);
	lua_bone::initialize(module);
	lua_textdraw::initialize(module);
	lua_txd::initialize(module);
	lua_component::initialize(module);
	lua_general::initialize(module);
	lua_shape::initialize(module);

	initializeEvents();

	return module;
}

SOL_MODULE_ENTRYPOINT(open);

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		// pin DLL to prevent unloading
		HMODULE module;
		GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN, reinterpret_cast<LPCWSTR>(&DllMain), &module);
	}
	return TRUE;
}
