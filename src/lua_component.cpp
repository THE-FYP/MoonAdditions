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
#include "lua_component.h"
#include "lua_texture.h"
#include "vehicle_renderer.h"
#include "game_sa/CPools.h"
#include "game_sa/CVehicle.h"
#include "game_sa/CClumpModelInfo.h"
#include "game_sa/CVisibilityPlugins.h"


namespace lua_component
{
	enum ComponentVisibilityState
	{
		VISIBILITY_DISABLED,
		VISIBILITY_ENABLED,
		VISIBILITY_DAMAGED,
	};

	namespace detail
	{
		struct DataStruct
		{
			sol::table* _table;
			CVehicle* _vehicle;
		};

		RwFrame* storeVehicleComponent(RwFrame* frame, void* data);
	}

	class VehicleMaterial
	{
	public:
		VehicleMaterial(CVehicle* veh, RpGeometry* geometry, RpMaterial* mat) :
			_vehicle(veh),
			_geometry(geometry),
			_material(mat)
		{
		}

		void setColor(CRGBA color)
		{
			VehicleRenderer::get()->setMaterialColor(_vehicle, _material, _geometry, {color.r, color.g, color.b, color.a});
		}

		CRGBA getColor() const
		{
			RwRGBA clr = _material->color;
			return CRGBA{clr.red, clr.green, clr.blue, clr.alpha};
		}

		void resetColor()
		{
			VehicleRenderer::get()->resetMaterialColor(_vehicle, _material);
		}

		void setTexture(std::shared_ptr<lua_texture::Texture> texture)
		{
			VehicleRenderer::get()->setMaterialTexture(_vehicle, _material, texture);
		}

		sol::optional<std::shared_ptr<lua_texture::Texture>> getTexture() const
		{
			if (!_material->texture)
				return sol::nullopt;
			return std::make_shared<lua_texture::Texture>(_material->texture, false);
		}

		void resetTexture()
		{
			VehicleRenderer::get()->resetMaterialTexture(_vehicle, _material);
		}

		uintptr_t getRawPointer() const
		{
			return reinterpret_cast<uintptr_t>(_material);
		}

		static VehicleMaterial fromMemory(uintptr_t vehicle, uintptr_t mat, uintptr_t geometry)
		{
			return VehicleMaterial{reinterpret_cast<CVehicle*>(vehicle), reinterpret_cast<RpGeometry*>(geometry), reinterpret_cast<RpMaterial*>(mat)};
		}

	private:
		CVehicle* _vehicle;
		RpGeometry* _geometry;
		RpMaterial* _material;
	}; // VehicleMaterial

	class VehicleAtomic
	{
	public:
		VehicleAtomic(CVehicle* veh, RpAtomic* atomic) :
			_vehicle(veh),
			_atomic(atomic)
		{
		}

		void hide(bool hide)
		{
			if (hide)
				_atomic->object.object.flags &= ~rpATOMICRENDER;
			else
				_atomic->object.object.flags |= rpATOMICRENDER;
		}

		bool getAtomicFlag(unsigned int flag) const
		{
			return (RpAtomicGetVisibilityPlugin(_atomic)->m_wFlags & flag) != 0;
		}

		void setAtomicFlag(unsigned short flag, bool value)
		{
			if (value)
				CVisibilityPlugins::SetAtomicFlag(_atomic, flag);
			else
				CVisibilityPlugins::ClearAtomicFlag(_atomic, flag);
		}

		sol::table getMaterials(sol::this_state ts) const
		{
			sol::table materials = sol::state_view{ts}.create_table();
			// yup
			std::pair<detail::DataStruct, RpGeometry*> data{{&materials, _vehicle}, _atomic->geometry};
			RpGeometryForAllMaterials(_atomic->geometry, [](RpMaterial* mat, void* data) {
				auto* ds = static_cast<std::pair<detail::DataStruct, RpGeometry*>*>(data);
				ds->first._table->add(VehicleMaterial{ds->first._vehicle, ds->second, mat});
				return mat;
			}, &data);
			return materials;
		}

		uintptr_t getRawPointer() const
		{
			return reinterpret_cast<uintptr_t>(_atomic);
		}

		static VehicleAtomic fromMemory(uintptr_t vehicle, uintptr_t atomic)
		{
			return VehicleAtomic{reinterpret_cast<CVehicle*>(vehicle), reinterpret_cast<RpAtomic*>(atomic)};
		}

	private:
		CVehicle* _vehicle;
		RpAtomic* _atomic;
	}; // VehicleAtomic

	class VehicleComponent
	{
	public:
		VehicleComponent(CVehicle* vehicle, RwFrame* component) :
			_vehicle(vehicle),
			_frame(component)
		{
		}

		std::string getName() const
		{
			// #pluginsdk
			return reinterpret_cast<const char*>(reinterpret_cast<uintptr_t>(_frame) + 0xAC);
		}

		CMatrix* getMatrix() const
		{
			return reinterpret_cast<CMatrix*>(RwFrameGetLTM(_frame));
		}

		CMatrix* getModelingMatrix() const
		{
			return reinterpret_cast<CMatrix*>(&_frame->modelling);
		}

		void setVisibilityState(ComponentVisibilityState state)
		{
			_vehicle->SetComponentVisibility(_frame, state);
		}

		void setAlpha(byte alpha)
		{
			RwFrameForAllObjects(_frame, reinterpret_cast<RwObjectCallBack>(CVehicle::SetComponentAtomicAlpha), reinterpret_cast<void*>(alpha));
		}

		sol::optional<VehicleComponent> getChild() const
		{
			if (_frame->child)
				return VehicleComponent{_vehicle, _frame->child};
			return sol::nullopt;
		}

		sol::optional<VehicleComponent> getNextComponent() const
		{
			if (_frame->next)
				return VehicleComponent{_vehicle, _frame->next};
			return sol::nullopt;
		}

		sol::optional<VehicleComponent> getParent() const
		{
			if (_frame->object.parent)
				return VehicleComponent{_vehicle, static_cast<RwFrame*>(_frame->object.parent)};
			return sol::nullopt;
		}

		sol::table getObjects(sol::this_state ts) const
		{
			sol::table objects = sol::state_view{ts}.create_table();
			detail::DataStruct data{&objects, _vehicle};
			RwFrameForAllObjects(_frame, [](RwObject* object, void* data) -> RwObject* {
				if (object->type == rpATOMIC)
				{
					auto* ds = static_cast<detail::DataStruct*>(data);
					ds->_table->add(VehicleAtomic{ds->_vehicle, reinterpret_cast<RpAtomic*>(object)});
				}
				return object;
			}, &data);
			return objects;
		}

		sol::table getChildComponents(sol::this_state ts) const
		{
			sol::table components = sol::state_view{ts}.create_table();
			detail::DataStruct data{&components, _vehicle};
			RwFrameForAllChildren(_frame, &detail::storeVehicleComponent, &data);
			return components;
		}

		uintptr_t getRawPointer() const
		{
			return reinterpret_cast<uintptr_t>(_frame);
		}

	private:
		CVehicle* _vehicle;
		RwFrame* _frame;
	}; // VehicleComponent

	namespace detail
	{
		RwFrame* storeVehicleComponent(RwFrame* frame, void* data)
		{
			auto ds = static_cast<DataStruct*>(data);
			ds->_table->add(VehicleComponent{ds->_vehicle, frame});
			return frame;
		}

		RwFrame* storeAllVehicleComponents(RwFrame* frame, void* data)
		{
			storeVehicleComponent(frame, data);
			return RwFrameForAllChildren(frame, &storeAllVehicleComponents, data);
		}
	}

	sol::optional<VehicleComponent> findVehicleComponent(unsigned int handle, const std::string& name)
	{
		CVehicle* vehicle = CPools::GetVehicle(handle);
		if (vehicle && vehicle->m_pRwClump && !name.empty())
		{
			auto component = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, const_cast<char*>(name.c_str()));
			if (component)
				return VehicleComponent{vehicle, component};
		}
		return sol::nullopt;
	}

	template<typename T, T cb>
	sol::table getVehicleComponents(sol::this_state ts, unsigned int handle)
	{
		sol::table components = sol::state_view{ts}.create_table();
		CVehicle* vehicle = CPools::GetVehicle(handle);
		if (vehicle && vehicle->m_pRwClump)
		{
			detail::DataStruct data{&components, vehicle};
			RwFrameForAllChildren(static_cast<RwFrame*>(vehicle->m_pRwClump->object.parent), cb, &data);
		}
		return components;
	}

	VehicleComponent vehicleComponentFromMemory(uintptr_t vehicle, uintptr_t frame)
	{
		return VehicleComponent{reinterpret_cast<CVehicle*>(vehicle), reinterpret_cast<RwFrame*>(frame)};
	}

	void initialize(sol::table& module)
	{
		module.set_function("get_vehicle_component", &findVehicleComponent);
		module.set_function("get_vehicle_components", &getVehicleComponents<decltype(&detail::storeVehicleComponent), &detail::storeVehicleComponent>);
		module.set_function("get_all_vehicle_components", &getVehicleComponents<decltype(&detail::storeAllVehicleComponents), &detail::storeAllVehicleComponents>);
		module.new_usertype<VehicleComponent>("vehicle_component", "new", sol::no_constructor,
											  "from_memory", sol::factories(&vehicleComponentFromMemory),
											  "get_pointer", &VehicleComponent::getRawPointer,
											  "name", sol::property(&VehicleComponent::getName),
											  "matrix", sol::property(&VehicleComponent::getMatrix),
											  "modeling_matrix", sol::property(&VehicleComponent::getModelingMatrix),
											  "set_visibility", &VehicleComponent::setVisibilityState,
											  "set_alpha", &VehicleComponent::setAlpha,
											  "child", sol::property(&VehicleComponent::getChild),
											  "parent", sol::property(&VehicleComponent::getParent),
											  "next", sol::property(&VehicleComponent::getNextComponent),
											  "get_objects", &VehicleComponent::getObjects,
											  "get_child_components", &VehicleComponent::getChildComponents);
		module.new_usertype<VehicleAtomic>("vehicle_atomic_object", "new", sol::no_constructor,
									  "from_memory", sol::factories(&VehicleAtomic::fromMemory),
									  "get_pointer", &VehicleAtomic::getRawPointer,
									  "hide", &VehicleAtomic::hide,
									  "get_atomic_flag", &VehicleAtomic::getAtomicFlag,
									  "set_atomic_flag", &VehicleAtomic::setAtomicFlag,
									  "get_materials", &VehicleAtomic::getMaterials);
		module.new_usertype<VehicleMaterial>("vehicle_object_material", "new", sol::no_constructor,
										"from_memory", sol::factories(&VehicleMaterial::fromMemory),
										"get_pointer", &VehicleMaterial::getRawPointer,
										"set_color", &VehicleMaterial::setColor,
										"get_color", &VehicleMaterial::getColor,
										"reset_color", &VehicleMaterial::resetColor,
										"set_texture", &VehicleMaterial::setTexture,
										"get_texture", &VehicleMaterial::getTexture,
										"reset_texture", &VehicleMaterial::resetTexture);
		module.new_enum<false>("component_state",
						"DISABLED", VISIBILITY_DISABLED,
						"ENABLED", VISIBILITY_ENABLED,
						"DAMAGED", VISIBILITY_DAMAGED);
	}
}
