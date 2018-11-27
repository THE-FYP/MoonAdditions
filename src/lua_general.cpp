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
#include "lua_general.h"
#include "optional.h"
#include "lua_texture.h"
#include "spotlight_drawer.h"
#include "sprite_drawer.h"
#include "game_sa/CPools.h"
#include "game_sa/CModelInfo.h"
#include "game_sa/CShadows.h"
#include "game_sa/CPointLights.h"
#include "game_sa/CCoronas.h"
#include "game_sa/CTimeCycle.h"
#include "game_sa/CWeather.h"
#include "game_sa/CProjectileInfo.h"
#include "game_sa/CMenuManager.h"
#include "game_sa/CWorld.h"
#include "game_sa/common.h"


namespace lua_general
{
	namespace detail
	{
		template<typename T>
		T* getEntityAtHandle(unsigned int handle);

		template<>
		CVehicle* getEntityAtHandle(unsigned int handle)
		{
			return CPools::GetVehicle(handle);
		}

		template<>
		CPed* getEntityAtHandle(unsigned int handle)
		{
			return CPools::GetPed(handle);
		}

		template<>
		CObject* getEntityAtHandle(unsigned int handle)
		{
			return CPools::GetObject(handle);
		}

		template<size_t sz>
		RwTexture* getRwTextureFromListOrLuaTexture(const sol::object& texture, uintptr_t const (&textures)[sz])
		{
			if (texture.get_type() == sol::type::number)
			{
				unsigned int id = texture.as<unsigned int>();
				if (id >= _countof(textures))
					return nullptr;
				return *reinterpret_cast<RwTexture**>(textures[id]);
			}
			else
			{
				auto tex = texture.as<std::shared_ptr<lua_texture::Texture>>();
				return tex->getRwTexture();
			}
		}

		RwTexture* getShadowTexture(const sol::object& texture)
		{
			uintptr_t textures[] = {0xC403F4, 0xC403E0, 0xC403E4, 0xC403F4, 0xC403E8, 0xC403F8, 0xC40400, 0xC403FC, 0xC403EC, 0xC403F0, 0xC4040C};
			return getRwTextureFromListOrLuaTexture(texture, textures);
		}

		RwTexture* getCoronaTexture(const sol::object& texture)
		{
			uintptr_t textures[] = {0xC3E000, 0xC3E004, 0xC3E008, 0xC3E00C, 0xC3E010, 0xC3E024};
			return getRwTextureFromListOrLuaTexture(texture, textures);
		}
	}
	
	template<typename T>
	bool setEntityModelAlpha(unsigned int handle, unsigned int alpha)
	{
		T* entity = detail::getEntityAtHandle<T>(handle);
		if (entity && entity->m_pRwObject)
		{
			entity->SetRwObjectAlpha(alpha);
			return true;
		}
		return false;
	}

	sol::optional<std::tuple<unsigned int, uintptr_t>> getEntityTypeAndClass(uintptr_t entity)
	{
		if (entity)
		{
			auto type = reinterpret_cast<CEntity*>(entity)->m_nType;
			return std::make_tuple(type, *reinterpret_cast<uintptr_t*>(entity));
		}
		return sol::nullopt;
	}

	template<typename T>
	int getEntityNumberOfCollidedEntities(unsigned int handle)
	{
		T* entity = detail::getEntityAtHandle<T>(handle);
		return entity ? entity->m_nNumEntitiesCollided : 0;
	}

	template<typename T>
	sol::table getEntityCollidedEntities(sol::this_state ts, unsigned int handle)
	{
		sol::table col = sol::state_view{ts}.create_table();
		T* entity = detail::getEntityAtHandle<T>(handle);
		if (entity)
		{
			for (auto* e : entity->m_apCollidedEntities)
				col.add(reinterpret_cast<uintptr_t>(e));
		}
		return col;
	}

	void setCarWindowState(unsigned int handle, unsigned char window, bool state)
	{
		CVehicle* vehicle = CPools::GetVehicle(handle);
		if (state)
			vehicle->SetWindowOpenFlag(window);
		else
			vehicle->ClearWindowOpenFlag(window);
	}

	float getActorMaxHelath(unsigned int handle)
	{
		CPed* ped = CPools::GetPed(handle);
		return ped ? ped->m_fMaxHealth : 0;
	}

	std::tuple<bool, float, float, float, float> getCirclesIntersection(float x1, float y1, float r1, float x2, float y2, float r2)
	{
		float dx = x2 - x1;
		float dy = y2 - y1;
		float d = hypot(dx, dy);
		if ((d > (r1 + r2)) || (d < fabs(r1 - r2)))
		{
			return {false, 0.0f, 0.0f, 0.0f, 0.0f};
		}
		else
		{
			float a = ((r1 * r1) - (r2 * r2) + (d * d)) / (2.0f * d);
			float x3 = x1 + (dx * a / d);
			float y3 = y1 + (dy * a / d);
			float h = sqrt((r1 * r1) - (a * a));
			float rx = -dy * (h / d);
			float ry = dx * (h / d);
			return {true, x3 + rx, y3 + ry, x3 - rx, y3 - ry};
		}
	}

	int getModelTxd(unsigned int modelId)
	{
		if (modelId >= 20000)
			return -1;
		CBaseModelInfo* model = CModelInfo::GetModelInfo(modelId);
		return model ? model->m_nTxdIndex : -1;
	}

	sol::optional<unsigned int> getModelCrc32(unsigned int modelId)
	{
		if (modelId >= 20000)
			return sol::nullopt;
		CBaseModelInfo* model = CModelInfo::GetModelInfo(modelId);
		if (!model)
			return sol::nullopt;
		return model->m_nKey;
	}

	using namespace optional;
	void addTemporaryShadow(unsigned int type, float x, float y, float z, float width, float height, float rotation, float dist, sol::object texture, unsigned int intensity,
							opt<byte> r, opt<byte> g, opt<byte> b, opt<uintptr_t> shadowData, opt<float> scale, opt<bool> drawOnBuildings, opt<bool> drawOnWater)
	{
		float a = rotation * 0.0174532925f;
		float a2 = (rotation + 90.0f) * 0.0174532925f;
		float x1 = cos(a) * width / 2.0f;
		float y1 = sin(a) * width / 2.0f;
		float x2 = cos(a2) * height / 2.0f;
		float y2 = sin(a2) * height / 2.0f;
		RwTexture* rwTexture = detail::getShadowTexture(texture);
		if (!rwTexture)
			return;
		CVector pos{x, y, z};
		CShadows::StoreShadowToBeRendered(type, rwTexture, &pos, x1, y1, x2, y2, intensity, r.value_or(255), g.value_or(255), b.value_or(255), dist, drawOnWater.value_or(false),
										  scale.value_or(1.0f), reinterpret_cast<CRealTimeShadow*>(shadowData.value_or(0)), drawOnBuildings.value_or(false));
	}

	void addPermanentShadow(unsigned int type, float x, float y, float z, float width, float height, float rotation, float dist, sol::object texture, unsigned int intensity,
							unsigned int time, opt<byte> r, opt<byte> g, opt<byte> b)
	{
		float a = rotation * 0.0174532925f;
		float a2 = (rotation + 90.0f) * 0.0174532925f;
		float x1 = cos(a) * width / 2.0f;
		float y1 = sin(a) * width / 2.0f;
		float x2 = cos(a2) * height / 2.0f;
		float y2 = sin(a2) * height / 2.0f;
		RwTexture* rwTexture = detail::getShadowTexture(texture);
		if (!rwTexture)
			return;
		CVector pos{x, y, z};
		CShadows::AddPermanentShadow(type, rwTexture, &pos, x1, y1, x2, y2, intensity, r.value_or(255), g.value_or(255), b.value_or(255), dist, time, 1.0f);
	}

	void addLight(unsigned int type, float x, float y, float z, float dirX, float dirY, float dirZ, float radius, opt<byte> r, opt<byte> g, opt<byte> b, opt<uintptr_t> affectEntity)
	{
		CPointLights::AddLight(type, {x, y, z}, {dirX, dirY, dirZ}, radius, (float)r.value_or(255) / 255.0f, (float)g.value_or(255) / 255.0f, (float)b.value_or(255) / 255.0f,
							   0, false, reinterpret_cast<CEntity*>(affectEntity.value_or(0)));
	}

	void addCorona(sol::object texture, float x, float y, float z, float size, opt<CRGBA> color, opt<uintptr_t> entity)
	{
		RwTexture* rwTexture = detail::getCoronaTexture(texture);
		if (!rwTexture)
			return;
		CVector pos{x, y, z};
		CRGBA clr = color.value_or(CRGBA{255, 255, 255, 255});
		// #todo fix flickering
		CCoronas::RegisterCorona(CCoronas::NumCoronas, reinterpret_cast<CEntity*>(entity.value_or(0)), clr.r, clr.g, clr.b, clr.a,
								 pos, size, 100.0f, rwTexture, FLARETYPE_NONE, false, false, 0, 0.0f, false, 0.15f, 0, 15.0f, false, false);
	}

	void addCoronaEx(sol::object texture, float x, float y, float z, float size, opt<CRGBA> color, opt<uintptr_t> entity, opt<float> farClip, opt<float> nearClip,
					 opt<eCoronaFlareType> flare, opt<bool> reflection, opt<bool> checkObstacles, opt<byte> flashWhileFading, opt<float> fadeSpeed, opt<bool> onlyFromBelow)
	{
		RwTexture* rwTexture = detail::getCoronaTexture(texture);
		if (!rwTexture)
			return;
		CVector pos{x, y, z};
		CRGBA clr = color.value_or(CRGBA{255, 255, 255, 255});
		CCoronas::RegisterCorona(CCoronas::NumCoronas, reinterpret_cast<CEntity*>(entity.value_or(0)), clr.r, clr.g, clr.b, clr.a,
								 pos, size, farClip.value_or(100.0f), rwTexture, flare.value_or(FLARETYPE_NONE), reflection.value_or(false),
								 checkObstacles.value_or(false), 0, 0.0f, false, nearClip.value_or(0.15f), flashWhileFading.value_or(0),
								 fadeSpeed.value_or(15.0f), onlyFromBelow.value_or(false), false);
	}

	std::tuple<int, int, int, int, int, int> getSunColors()
	{
		unsigned char* color = CTimeCycle::m_nSunCoreRed;
		return {color[0], color[1], color[2], color[3], color[4], color[5]};
	}

	std::tuple<float, float> getSunScreenCoords()
	{
		return {CCoronas::SunScreenX, CCoronas::SunScreenY};
	}

	std::tuple<bool, float, float, float> getSunWorldCoords()
	{
		const float sunDistance = 10000.0f;
		CVector* sunPos = reinterpret_cast<CVector*>(*reinterpret_cast<uint32_t*>(0xB79FD0) * 12 + 0xB7CA50);
		return {sunPos->z > -0.1f, sunPos->x * sunDistance, sunPos->y * sunDistance, sunPos->z * sunDistance};
	}
	
	std::tuple<float, float> getSunSize()
	{
		float size = *CTimeCycle::m_fSunSize;
		return {size * 2.7335f, size * 6.0f};
	}

	int getWeather()
	{
		return CWeather::OldWeatherType;
	}

	std::tuple<byte, byte> getTraffLightsColor()
	{
		byte ns = ((byte(__cdecl*)())0x49D3A0)();
		byte we = ((byte(__cdecl*)())0x49D350)();
		return {ns, we};
	}

	sol::optional<unsigned int> getCarLightDamageStatus(unsigned int carHandle, eLights lightId)
	{
		CAutomobile* vehicle = static_cast<CAutomobile*>(CPools::GetVehicle(carHandle));
		if (!vehicle)
			return sol::nullopt;
		return vehicle->m_damageManager.GetLightStatus(lightId);
	}

	void setCarLightDamageStatus(unsigned int carHandle, eLights lightId, unsigned int status)
	{
		CAutomobile* vehicle = static_cast<CAutomobile*>(CPools::GetVehicle(carHandle));
		if (!vehicle)
			return;
		vehicle->m_damageManager.SetLightStatus(lightId, status);
	}
	
	sol::optional<std::tuple<unsigned int, unsigned int>> getVehicleClass(unsigned int handle)
	{
		CVehicle* vehicle = CPools::GetVehicle(handle);
		if (!vehicle)
			return sol::nullopt;
		return std::make_tuple(vehicle->m_nVehicleClass, vehicle->m_nVehicleSubClass);
	}

	std::tuple<float, float, float> getVehicleDummyPosition(unsigned int carHandle, unsigned int element, opt<bool> world, opt<bool> invertX)
	{
		CVector pos{0.0f, 0.0f, 0.0f};
		if (element < 15)
		{
			CVehicle* vehicle = CPools::GetVehicle(carHandle);
			if (vehicle)
			{
				auto vehModel = static_cast<CVehicleModelInfo*>(CModelInfo::GetModelInfo(vehicle->m_nModelIndex));
				if (vehModel && vehModel->m_pVehicleStruct)
				{
					pos = vehModel->m_pVehicleStruct->m_avDummyPos[element];
					if (invertX.value_or(false))
						pos.x *= -1.0f;
					if (world.value_or(false))
						pos = vehicle->TransformFromObjectSpace(pos);
				}
			}
		}
		return {pos.x, pos.y, pos.z};
	}

	bool createProjectile(eWeaponType type, float srcX, float srcY, float srcZ, float dirX, float dirY, float dirZ, float force, uintptr_t entity, uintptr_t targetEntity)
	{
		CVector direction(dirX, dirY, dirZ);
		return CProjectileInfo::AddProjectile(reinterpret_cast<CEntity*>(entity), type, {srcX, srcY, srcZ}, force, &direction, reinterpret_cast<CEntity*>(targetEntity));
	}

	std::tuple<char, char> getGameVolume()
	{
		return {FrontEndMenuManager.m_nSfxVolume, FrontEndMenuManager.m_nRadioVolume};
	}

	sol::optional<sol::table> getCollisionBetweenPoints(sol::this_state ts, float x1, float y1, float z1, float x2, float y2, float z2, opt<sol::table> flags, opt<uintptr_t> ignoringEntity)
	{
		CVector origin{x1, y1, z1}, target{x2, y2, z2};
		bool buildings = true, 
			vehicles = false,
			peds = false,
			objects = true,
			dummies = false,
			seeThroughCheck = false,
			ignoreSomeObjectsCheck = false, 
			shootThroughCheck = false;
		if (flags)
		{
			auto setFlag = [&f = flags.value()](const char* name, bool& flag) { optionalSet(f.traverse_get<opt<bool>>(name), flag); };
			setFlag("vehicles", vehicles);
			setFlag("peds", peds);
			setFlag("buildings", buildings);
			setFlag("objects", objects);
			setFlag("dummies", dummies);
			setFlag("see_through", seeThroughCheck);
			setFlag("shoot_through", shootThroughCheck);
			setFlag("ignore_some_objects", ignoreSomeObjectsCheck);
		}

		auto oldIgnoredEntity = CWorld::pIgnoreEntity;
		CWorld::pIgnoreEntity = reinterpret_cast<CEntity*>(ignoringEntity.value_or(0));
		CColPoint colPoint;
		CEntity* colEntity;
		if (!CWorld::ProcessLineOfSight(origin, target, colPoint, colEntity, buildings, vehicles, peds, objects, dummies, seeThroughCheck, ignoreSomeObjectsCheck, shootThroughCheck))
		{
			CWorld::pIgnoreEntity = oldIgnoredEntity;
			return sol::nullopt;
		}
		CWorld::pIgnoreEntity = oldIgnoredEntity;
		
		sol::table result = sol::state_view{ts}.create_table_with(
			"entity", reinterpret_cast<uintptr_t>(colEntity),
			"position", colPoint.m_vecPoint,
			"normal", colPoint.m_vecNormal,
			"depth", colPoint.m_fDepth,
			"lighting_a", colPoint.m_nLightingA,
			"lighting_b", colPoint.m_nLightingB,
			"piece_type_a", colPoint.m_nPieceTypeA,
			"piece_type_b", colPoint.m_nPieceTypeB,
			"surface_type_a", colPoint.m_nSurfaceTypeA,
			"surface_type_b", colPoint.m_nSurfaceTypeB
		);
		return result;
	}
	
	void drawSpotlight(float fromX, float fromY, float fromZ, float toX, float toY, float toZ, float baseRadius, float targetRadius, opt<bool> shadow, opt<float> shadowIntensity)
	{
		SpotlightDrawer::get()->addSpotlight(CVector{fromX, fromY, fromZ}, CVector{toX, toY, toZ}, baseRadius, targetRadius, shadow.value_or(true), shadowIntensity.value_or(1.0f));
	}

	void drawRect(CRect rect, CRGBA color, sol::optional<float> angle)
	{
		SpriteDrawer::get()->add(nullptr, rect, color, color, color, color, angle.value_or(0.0f) / 57.2957795f);
	}

	void drawRectWithGradient(CRect rect, CRGBA color1, CRGBA color2, CRGBA color3, CRGBA color4, sol::optional<float> angle)
	{
		SpriteDrawer::get()->add(nullptr, rect, color1, color2, color3, color4, angle.value_or(0.0f) / 57.2957795f);
	}

	template<typename ObjT, typename MT = ObjT, typename PredT>
	sol::table getAllEntities(sol::state_view lua, float x, float y, float z, opt<float> radius, opt<bool> sphere, CPool<ObjT, MT>* pool, PredT pred)
	{
		sol::table objects = lua.create_table();
		CVector coords{x, y, z};
		for (int i = 0; i < pool->m_nSize; ++i)
		{
			ObjT* ent = pool->GetAt(i);
			if (ent && pred(ent))
			{
				CVector diff = coords - ent->GetPosition();
				if ((sphere.value_or(false) ? diff.Magnitude2D() : diff.Magnitude()) <= radius.value_or(1000.0f))
					objects.add(pool->GetRef(ent));
			}
		}
		return objects;
	}

	sol::table getAllObjects(sol::this_state ts, float x, float y, float z, opt<float> radius, opt<bool> sphere)
	{
		return getAllEntities(ts, x, y, z, radius, sphere, CPools::ms_pObjectPool, [](CObject*) { return true; });
	}

	sol::table getAllPeds(sol::this_state ts, float x, float y, float z, opt<float> radius, opt<bool> skipDead, opt<bool> sphere)
	{
		return getAllEntities(ts, x, y, z, radius, sphere, CPools::ms_pPedPool, [skipDead](CPed* ped) {
			if (ped->m_nPedFlags.bFadeOut || ped == FindPlayerPed())
				return false;
			return !skipDead.value_or(false) || ped->IsAlive();
		});
	}

	sol::table getAllVehicles(sol::this_state ts, float x, float y, float z, opt<float> radius, opt<bool> skipWrecked, opt<bool> sphere)
	{
		return getAllEntities(ts, x, y, z, radius, sphere, CPools::ms_pVehiclePool, [skipWrecked](CVehicle* veh) {
			if (veh->m_nFlags.bFadeOut)
				return false;
			return !skipWrecked.value_or(false) || veh->m_nStatus == STATUS_WRECKED;
		});
	}

	template<typename T>
	sol::optional<CMatrix*> getEntityMatrix(unsigned int handle)
	{
		T* entity = detail::getEntityAtHandle<T>(handle);
		if (entity && entity->m_matrix)
		{
			return static_cast<CMatrix*>(entity->m_matrix);
		}
		return sol::nullopt;
	}

	void initialize(sol::table& module)
	{
		module.set_function("set_vehicle_model_alpha", &setEntityModelAlpha<CVehicle>);
		module.set_function("set_char_model_alpha", &setEntityModelAlpha<CPed>);
		module.set_function("set_object_model_alpha", &setEntityModelAlpha<CObject>);
		module.set_function("get_entity_type_and_class", &getEntityTypeAndClass);
		module.set_function("get_vehicle_number_of_collided_entites", &getEntityNumberOfCollidedEntities<CVehicle>);
		module.set_function("get_char_number_of_collided_entites", &getEntityNumberOfCollidedEntities<CPed>);
		module.set_function("get_object_number_of_collided_entites", &getEntityNumberOfCollidedEntities<CObject>);
		module.set_function("get_vehicle_collided_entites", &getEntityCollidedEntities<CVehicle>);
		module.set_function("get_char_collided_entites", &getEntityCollidedEntities<CPed>);
		module.set_function("get_object_collided_entites", &getEntityCollidedEntities<CObject>);
		module.set_function("set_car_window_state", &setCarWindowState);
		module.set_function("get_char_max_health", &getActorMaxHelath);
		module.set_function("get_circles_intersection", &getCirclesIntersection);
		module.set_function("get_model_txd_id", &getModelTxd);
		module.set_function("get_model_crc32_key", &getModelCrc32);
		module.set_function("draw_temporary_shadow", &addTemporaryShadow);
		module.set_function("draw_permanent_shadow", &addPermanentShadow);
		module.set_function("draw_light", &addLight);
		module.set_function("draw_corona", &addCorona);
		module.set_function("draw_corona_ex", &addCoronaEx);
		module.set_function("get_sun_colors", &getSunColors);
		module.set_function("get_sun_size", &getSunSize);
		module.set_function("get_sun_screen_coordinates", &getSunScreenCoords);
		module.set_function("get_sun_world_position", &getSunWorldCoords);
		module.set_function("get_current_weather", &getWeather);
		module.set_function("get_traffic_lights_color", &getTraffLightsColor);
		module.set_function("get_car_light_damage_status", &getCarLightDamageStatus);
		module.set_function("set_car_light_damage_status", &setCarLightDamageStatus);
		module.set_function("get_vehicle_class", &getVehicleClass);
		module.set_function("get_vehicle_dummy_element_position", &getVehicleDummyPosition);
		module.set_function("create_projectile", &createProjectile);
		module.set_function("get_game_volume", &getGameVolume);
		module.set_function("get_collision_between_points", &getCollisionBetweenPoints);
		module.set_function("draw_spotlight", &drawSpotlight);
		module.set_function("draw_rect", &drawRect);
		module.set_function("draw_rect_with_gradient", &drawRectWithGradient);
		module.set_function("get_all_objects", &getAllObjects);
		module.set_function("get_all_characters", &getAllPeds);
		module.set_function("get_all_vehicles", &getAllVehicles);
		module.set_function("get_object_matrix", &getEntityMatrix<CObject>);
		module.set_function("get_char_matrix", &getEntityMatrix<CPed>);
		module.set_function("get_vehicle_matrix", &getEntityMatrix<CVehicle>);
		module.new_enum<false>("entity_type",
						"NOTHING", ENTITY_TYPE_NOTHING,
						"BUILDING", ENTITY_TYPE_BUILDING,
						"VEHICLE", ENTITY_TYPE_VEHICLE,
						"PED", ENTITY_TYPE_PED,
						"OBJECT", ENTITY_TYPE_OBJECT,
						"DUMMY", ENTITY_TYPE_DUMMY,
						"NOTINPOOLS", ENTITY_TYPE_NOTINPOOLS);
		module.new_enum<false>("entity_class",
						"ENTITY", 0x863928,
						"BUILDING", 0x8585C8,
						"TREADABLE", 0x8639B0,
						"DUMMY_1", 0x8638C0,
						"DUMMY_2", 0x86C198,
						"PHYSICAL", 0x863BA0,
						"OBJECT", 0x866F60,
						"HAND", 0x866EE0,
						"PROJECTILE", 0x867030,
						"CUTSCENE_OBJECT", 0x868A60,
						"PED", 0x86C358,
						"CIVILIAN_PED", 0x86C0A8,
						"COP_PED", 0x86C120,
						"EMERGENCY_PED", 0x86C200,
						"PLAYER_PED", 0x86D168,
						"VEHICLE", 0x871E80,
						"AUTOMOBILE", 0x871120,
						"HELI", 0x871680,
						"MTRUCK", 0x8717D8,
						"PLANE", 0x871948,
						"QUAD", 0x871AE8,
						"TRAILER", 0x871C28,
						"BIKE", 0x871360,
						"BMX", 0x871528,
						"BOAT", 0x8721A0,
						"TRAIN", 0x872370);
		module.new_enum<false>("vehicle_window",
						"FRONT_LEFT", 10,
						"FRONT_RIGHT", 8,
						"REAR_LEFT", 11,
						"REAR_RIGHT", 9);
		module.new_enum<false>("shadow_type",
						"DEFAULT", SHADOW_DEFAULT,
						"ADDITIVE", SHADOW_ADDITIVE,
						"INVCOLOR", SHADOW_INVCOLOR,
						"OIL_1", SHADOW_OIL_1,
						"OIL_2", SHADOW_OIL_2,
						"OIL_3", SHADOW_OIL_3,
						"OIL_4", SHADOW_OIL_4,
						"OIL_5", SHADOW_OIL_5);
		module.new_enum<false>("shadow_texture",
						"DEFAULT", 0,
						"CAR", 1,
						"PED", 2,
						"EXPLOSION", 3,
						"HELI", 4,
						"HEADLIGHT", 5,
						"BLOODPOOL", 6,
						"HEADLIGHT", 7,
						"BIKE", 8,
						"RCBARON", 9,
						"LAMP", 10);
		module.new_enum<false>("light_type",
						"DEFAULT", 0,
						"DIRECTIONAL", 1,
						"DARK", 2,
						"GLOBAL", 3);
		module.new_enum<false>("corona_texture",
						"STAR", 0,
						"STAR_2", 1,
						"MOON", 2,
						"REFLECT", 3,
						"HEADLIGHT_LINE", 4,
						"RINGB", 5);
		module.new_enum<false>("car_light",
						"FRONT_LEFT", LIGHT_FRONT_LEFT,
						"FRONT_RIGHT", LIGHT_FRONT_RIGHT,
						"REAR_RIGHT", LIGHT_REAR_RIGHT,
						"REAR_LEFT", LIGHT_REAR_LEFT);
		module.new_enum<false>("vehicle_class",
						"AUTOMOBILE", VEHICLE_AUTOMOBILE,
						"MTRUCK", VEHICLE_MTRUCK,
						"QUAD", VEHICLE_QUAD,
						"HELI", VEHICLE_HELI,
						"PLANE", VEHICLE_PLANE,
						"BOAT", VEHICLE_BOAT,
						"TRAIN", VEHICLE_TRAIN,
						"FHELI", VEHICLE_FHELI,
						"FPLANE", VEHICLE_FPLANE,
						"BIKE", VEHICLE_BIKE,
						"BMX", VEHICLE_BMX,
						"TRAILER", VEHICLE_TRAILER);
		module.new_enum<false>("vehicle_subclass",
						"AUTOMOBILE", VEHICLE_APPEARANCE_AUTOMOBILE,
						"BIKE", VEHICLE_APPEARANCE_BIKE,
						"HELI", VEHICLE_APPEARANCE_HELI,
						"BOAT", VEHICLE_APPEARANCE_BOAT,
						"PLANE", VEHICLE_APPEARANCE_PLANE);
		module.new_enum<false>("vehicle_dummy",
						"HEADLIGHTS", 0,
						"TAIL_LIGHTS", 1,
						"HEADLIGHTS_2", 2,
						"TAIL_LIGHTS_2", 3,
						"PED_FRONT_SEAT", 4,
						"PED_BACK_SEAT", 5,
						"EXHAUST", 6,
						"ENGINE", 7,
						"PETROLCAP", 8,
						"HOOKUP", 9,
						"PED_ARM", 10);
		module.new_enum<false>("collision_check_flag",
						"VEHICLES", "vehicles",
						"PEDS", "peds",
						"BUILDINGS", "buildings",
						"OBJECTS", "objects",
						"DUMMIES", "dummies",
						"SEE_THROUGH", "see_through",
						"SHOOT_THROUGH", "shoot_through",
						"IGNORE_SOME_OBJECTS", "ignore_some_objects");
	}
}
