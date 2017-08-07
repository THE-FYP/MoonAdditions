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
#include "game_sa\CRect.h"
#include "game_sa\CRGBA.h"
#include "game_sa\RenderWare.h"
#include "drawer.h"
#include "singleton.h"
#include "forward_declarations.h"


class Sprite
{
public:
	Sprite(std::shared_ptr<const lua_texture::Texture> texture, const CRect& rect, const CRGBA& color1, const CRGBA& color2, const CRGBA& color3, const CRGBA& color4, float angle) :
		_texture(texture),
		_rect(rect),
		_color1(color1), _color2(color2), _color3(color3), _color4(color4),
		_angle(angle),
		_empty(texture == nullptr)
	{
	}

	void draw() const;

private:
	std::weak_ptr<const lua_texture::Texture> _texture;
	CRect _rect;
	CRGBA _color1, _color2, _color3, _color4;
	float _angle;
	bool _empty;
};

using SpriteDrawer = Singleton<Drawer<Sprite>>;
