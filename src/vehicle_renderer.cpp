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
#include "vehicle_renderer.h"
#include "lua_texture.h"


auto& VehicleRenderer::getVehicleData(CVehicle* veh)
{
	if (!_vehicleData)
		_vehicleData = std::make_unique<VehicleDataExtended<VehicleData>>();
	return _vehicleData->get(veh);
}

auto& VehicleRenderer::getVehicleMaterialProperties(CVehicle* veh)
{
	return getVehicleData(veh)._materialProperties;
}

void VehicleRenderer::setMaterialColor(CVehicle* veh, RpMaterial* material, RpGeometry* geometry, RwRGBA color)
{
	auto& matProps = getVehicleMaterialProperties(veh)[material];
	matProps._recolor = true;
	matProps._color = color;
	matProps._geometry = geometry;
}

void VehicleRenderer::setMaterialTexture(CVehicle* veh, RpMaterial* material, std::shared_ptr<lua_texture::Texture> texture)
{
	auto& matProps = getVehicleMaterialProperties(veh)[material];
	matProps._retexture = true;
	matProps._texture = texture;
}

void VehicleRenderer::resetMaterialColor(CVehicle* veh, RpMaterial* material)
{
	if (isInitialized() && _vehicleData->exists(veh))
	{
		auto& matProps = getVehicleMaterialProperties(veh)[material];
		matProps._recolor = false;
		matProps._color = {0, 0, 0, 0};
	}
}

void VehicleRenderer::resetMaterialTexture(CVehicle* veh, RpMaterial* material)
{
	if (isInitialized() && _vehicleData->exists(veh))
	{
		auto& matProps = getVehicleMaterialProperties(veh)[material];
		matProps._retexture = false;
		matProps._texture.reset();
	}
}

void VehicleRenderer::setMaterialAmbient(CVehicle* veh, RpMaterial* material, RwReal ambient)
{
	auto& matProps = getVehicleMaterialProperties(veh)[material];
	matProps._reAmbient = true;
	matProps._ambient = ambient;
}

void VehicleRenderer::setMaterialDiffuse(CVehicle* veh, RpMaterial* material, RwReal diffuse)
{
	auto& matProps = getVehicleMaterialProperties(veh)[material];
	matProps._reDiffuse = true;
	matProps._diffuse = diffuse;
}

void VehicleRenderer::setMaterialSpecular(CVehicle* veh, RpMaterial* material, RwReal specular)
{
	auto& matProps = getVehicleMaterialProperties(veh)[material];
	matProps._reSpecular = true;
	matProps._specular = specular;
}

void VehicleRenderer::resetMaterialAmbient(CVehicle* veh, RpMaterial* material)
{
	if (isInitialized() && _vehicleData->exists(veh))
	{
		auto& props = _vehicleData->get(veh)._materialProperties;
		auto it = props.find(material);
		if (it != props.end())
		{
			it->second._reAmbient = false;
		}
	}
}

void VehicleRenderer::resetMaterialDiffuse(CVehicle* veh, RpMaterial* material)
{
	if (isInitialized() && _vehicleData->exists(veh))
	{
		auto& props = _vehicleData->get(veh)._materialProperties;
		auto it = props.find(material);
		if (it != props.end())
		{
			it->second._reDiffuse = false;
		}
	}
}

void VehicleRenderer::resetMaterialSpecular(CVehicle* veh, RpMaterial* material)
{
	if (isInitialized() && _vehicleData->exists(veh))
	{
		auto& props = _vehicleData->get(veh)._materialProperties;
		auto it = props.find(material);
		if (it != props.end())
		{
			it->second._reSpecular = false;
		}
	}
}

void VehicleRenderer::setMaterialSurfaceProperties(CVehicle* veh, RpMaterial* material, RwReal ambient, RwReal diffuse, RwReal specular)
{
	auto& matProps = getVehicleMaterialProperties(veh)[material];
	matProps._reAmbient = true;
	matProps._ambient = ambient;
	matProps._reDiffuse = true;
	matProps._diffuse = diffuse;
	matProps._reSpecular = true;
	matProps._specular = specular;
}

void VehicleRenderer::resetMaterialSurfaceProperties(CVehicle* veh, RpMaterial* material)
{
	if (isInitialized() && _vehicleData->exists(veh))
	{
		auto& props = _vehicleData->get(veh)._materialProperties;
		auto it = props.find(material);
		if (it != props.end())
		{
			it->second._reAmbient = false;
			it->second._reDiffuse = false;
			it->second._reSpecular = false;
		}
	}
}


const VehicleRenderer::MaterialProperties* VehicleRenderer::getMaterialProperties(CVehicle* veh, RpMaterial* material) const
{
	if (_vehicleData && _vehicleData->exists(veh))
	{
		auto& props = _vehicleData->get(veh)._materialProperties;
		auto it = props.find(material);
		if (it != props.end())
			return &it->second;
	}
	return nullptr;
}



void VehicleRenderer::processRender(CVehicle* veh)
{
	if (isInitialized() && _vehicleData->exists(veh))
	{
		for (auto& it : _vehicleData->get(veh)._materialProperties)
		{
			if (it.second._recolor)
			{
				it.second._originalColor = it.first->color;
				it.first->color = it.second._color;
				it.second._originalGeometryFlags = it.second._geometry->flags;
				it.second._geometry->flags |= rpGEOMETRYMODULATEMATERIALCOLOR;
			}
			if (it.second._retexture)
			{
				auto tex = it.second._texture.lock();
				if (tex)
				{
					it.second._originalTexture = it.first->texture;
					it.first->texture = tex->getRwTexture();
				}
				else
				{
					it.second._retexture = false;
				}
			}

			if (it.second._reAmbient)
			{
				it.second._originalAmbient = it.first->surfaceProps.ambient;
				it.first->surfaceProps.ambient = it.second._ambient;
			}
			if (it.second._reDiffuse)
			{
				it.second._originalDiffuse = it.first->surfaceProps.diffuse;
				it.first->surfaceProps.diffuse = it.second._diffuse;
			}
			if (it.second._reSpecular)
			{
				it.second._originalSpecular = it.first->surfaceProps.specular;
				it.first->surfaceProps.specular = it.second._specular;
			}
		}
	}
}

void VehicleRenderer::postRender(CVehicle* veh)
{
	if (isInitialized() && _vehicleData->exists(veh))
	{
		for (auto& it : _vehicleData->get(veh)._materialProperties)
		{
			if (it.second._recolor)
			{
				it.first->color = it.second._originalColor;
				it.second._geometry->flags = it.second._originalGeometryFlags;
			}
			if (it.second._retexture)
			{
				it.first->texture = it.second._originalTexture;
			}
			if (it.second._reAmbient)
			{
				it.first->surfaceProps.ambient = it.second._originalAmbient;
			}
			if (it.second._reDiffuse)
			{
				it.first->surfaceProps.diffuse = it.second._originalDiffuse;
			}
			if (it.second._reSpecular)
			{
				it.first->surfaceProps.specular = it.second._originalSpecular;
			}
		}
	}
}
