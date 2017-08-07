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
#include "lua_texture.h"
#include "texture_manager.h"
#include "sprite_drawer.h"


namespace lua_texture
{
	Texture::~Texture()
	{
		if (_rwTexture)
		{
			if (_allocated)
				texture_manager::unloadTexture(_rwTexture);
			_rwTexture = nullptr;
		}
	}

	int Texture::getWidth() const
	{
		if (!isValid())
			return 0;
		return _rwTexture->raster->originalWidth;
	}

	int Texture::getHeight() const
	{
		if (!isValid())
			return 0;
		return _rwTexture->raster->originalHeight;
	}

	void Texture::draw(CRect rect, sol::optional<CRGBA> color, sol::optional<float> angle) const
	{
		if (isValid())
		{
			CRGBA clr = color.value_or(CRGBA{255, 255, 255, 255});
			SpriteDrawer::get()->add(shared_from_this(), rect, clr, clr, clr, clr, angle.value_or(0.0f) / 57.2957795f);
		}
	}

	void Texture::drawWithGradient(CRect rect, sol::optional<CRGBA> color1, sol::optional<CRGBA> color2, sol::optional<CRGBA> color3, sol::optional<CRGBA> color4, sol::optional<float> angle)
	{
		if (isValid())
		{
			const CRGBA defaultColor{255, 255, 255, 255};
			CRGBA clr1 = color1.value_or(defaultColor);
			CRGBA clr2 = color2.value_or(defaultColor);
			CRGBA clr3 = color3.value_or(defaultColor);
			CRGBA clr4 = color4.value_or(defaultColor);
			SpriteDrawer::get()->add(shared_from_this(), rect, clr1, clr2, clr3, clr4, angle.value_or(0.0f) / 57.2957795f);
		}
	}

	sol::optional<std::tuple<std::string, std::string>> Texture::getName() const
	{
		if (isValid())
			return std::make_tuple(_rwTexture->name, _rwTexture->mask);
		else
			return sol::nullopt;
	}

	sol::optional<std::shared_ptr<Texture>> Texture::loadBmpWithAlpha(const std::string& imagePath, const std::string& maskPath)
	{
		RwTexture* texture = texture_manager::loadMaskedTextureFromBmpFile(imagePath, maskPath);
		if (texture)
			return std::make_shared<Texture>(texture, true);
		return sol::nullopt;
	}

	sol::optional<std::shared_ptr<Texture>> Texture::loadPng(const std::string& file)
	{
		RwTexture* texture = texture_manager::loadTextureFromPngFile(file);
		if (texture)
			return std::make_shared<Texture>(texture, true);
		return sol::nullopt;
	}

	sol::optional<std::shared_ptr<Texture>> Texture::loadDds(const std::string& file)
	{
		RwTexture* texture = texture_manager::loadTextureFromDdsFile(file);
		if (texture)
			return std::make_shared<Texture>(texture, true);
		return sol::nullopt;
	}

	std::shared_ptr<Texture> textureFromMemory(uintptr_t texture)
	{
		return std::make_shared<Texture>(reinterpret_cast<RwTexture*>(texture), false);
	}

	void lua_texture::initialize(sol::table& module)
	{
		module.set_function("load_bmp_texture_with_mask", &Texture::loadBmpWithAlpha);
		module.set_function("load_png_texture", &Texture::loadPng);
		module.set_function("load_dds_texture", &Texture::loadDds);
		module.new_usertype<Texture>("texture", "new", sol::no_constructor,
									 "from_memory", sol::factories(&textureFromMemory),
									 "get_pointer", &Texture::getRawPointer,
									 "draw", &Texture::draw,
									 "draw_with_gradient", &Texture::drawWithGradient,
									 "width", sol::property(&Texture::getWidth),
									 "height", sol::property(&Texture::getHeight),
									 "name", sol::property(&Texture::getName),
									 "valid", sol::property(&Texture::isValid));
	}
}
