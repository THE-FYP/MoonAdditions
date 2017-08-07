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
#include "game_sa/RenderWare.h"


namespace lua_texture
{
	class Texture : public std::enable_shared_from_this<Texture>
	{
	public:
		Texture(RwTexture* texture, bool allocated) :
			_rwTexture(texture),
			_allocated(allocated)
		{
		}
		~Texture();

		bool isValid() const { return _rwTexture != nullptr && _rwTexture->raster != nullptr; }
		RwTexture* getRwTexture() const { return _rwTexture; }
		uintptr_t getRawPointer() const { return reinterpret_cast<uintptr_t>(_rwTexture); }
		int getWidth() const;
		int getHeight() const;
		void draw(CRect rect, sol::optional<CRGBA> color, sol::optional<float> angle) const;
		void drawWithGradient(CRect rect, sol::optional<CRGBA> color1, sol::optional<CRGBA> color2, sol::optional<CRGBA> color3, sol::optional<CRGBA> color4, sol::optional<float> angle);
		sol::optional<std::tuple<std::string, std::string>> getName() const;

		static sol::optional<std::shared_ptr<Texture>> loadBmpWithAlpha(const std::string& imagePath, const std::string& maskPath);
		static sol::optional<std::shared_ptr<Texture>> loadPng(const std::string& file);
		static sol::optional<std::shared_ptr<Texture>> loadDds(const std::string& file);

	private:
		RwTexture* _rwTexture;
		bool _allocated;
	};

	void initialize(sol::table& module);
}
