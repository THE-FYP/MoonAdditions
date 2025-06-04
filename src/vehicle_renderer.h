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

#pragma once
#include "singleton.h"
#include "extender/VehicleExtender_SA.h"
#include "forward_declarations.h"
#include "pool_object_extender.h"


class VehicleRenderer : public Singleton<VehicleRenderer>
{
	struct MaterialProperties
	{
		MaterialProperties() :
			_color{ 0, 0, 0, 0 },
			_ambient(0.5f),
			_diffuse(1.0f),
			_specular(1.0f),
			_recolor(false),
			_retexture(false),
			_reAmbient(false),
			_reDiffuse(false),
			_reSpecular(false),
			_originalColor{ 0, 0, 0, 0 },
			_originalTexture(nullptr),
			_originalAmbient(0.5f),
			_originalDiffuse(1.0f),
			_originalSpecular(1.0f),
			_originalGeometryFlags(0),
			_geometry(nullptr)
		{
		}

		RwRGBA _color;
		std::weak_ptr<lua_texture::Texture> _texture;
		bool _recolor;
		bool _retexture;
		RpGeometry* _geometry;
		RwRGBA _originalColor;
		RwTexture* _originalTexture;
		RwInt32 _originalGeometryFlags;
		RwReal _ambient;
		bool _reAmbient;
		RwReal _diffuse;
		bool _reDiffuse;
		RwReal _specular;
		bool _reSpecular;
		RwReal _originalAmbient;
		RwReal _originalDiffuse;
		RwReal _originalSpecular;
	};

	struct VehicleData
	{
		VehicleData(const CVehicle* veh)
		{
		}

		std::unordered_map<RpMaterial*, MaterialProperties> _materialProperties;
	};

public:
	void setMaterialColor(CVehicle* veh, RpMaterial* material, RpGeometry* geometry, RwRGBA color);
	void setMaterialTexture(CVehicle* veh, RpMaterial* material, std::shared_ptr<lua_texture::Texture> texture);
	void resetMaterialColor(CVehicle* veh, RpMaterial* material);
	void resetMaterialTexture(CVehicle* veh, RpMaterial* material);
	void setMaterialAmbient(CVehicle* veh, RpMaterial* material, RwReal ambient);
	void setMaterialDiffuse(CVehicle* veh, RpMaterial* material, RwReal diffuse);
	void setMaterialSpecular(CVehicle* veh, RpMaterial* material, RwReal specular);
	void resetMaterialAmbient(CVehicle* veh, RpMaterial* material);
	void resetMaterialDiffuse(CVehicle* veh, RpMaterial* material);
	void resetMaterialSpecular(CVehicle* veh, RpMaterial* material);
	const MaterialProperties* getMaterialProperties(CVehicle* veh, RpMaterial* material) const;
	void setMaterialSurfaceProperties(CVehicle* veh, RpMaterial* material, RwReal ambient, RwReal diffuse, RwReal specular);
	void resetMaterialSurfaceProperties(CVehicle* veh, RpMaterial* material);
	void processRender(CVehicle* veh);
	void postRender(CVehicle* veh);
	bool isInitialized() const { return _vehicleData != nullptr; }

private:
	auto& getVehicleMaterialProperties(CVehicle* veh);
	auto& getVehicleData(CVehicle* veh);

	std::unique_ptr<VehicleDataExtended<VehicleData>> _vehicleData;
};
