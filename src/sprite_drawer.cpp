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
#include "sprite_drawer.h"
#include "game_sa\CSprite2d.h"


void rotateVertices(CVector2D* rect, unsigned int numVerts, float x, float y, float angle)
{
	float xold, yold;
	float _cos = cos(angle);
	float _sin = sin(angle);
	for (unsigned int i = 0; i < numVerts; i++)
	{
		xold = rect[i].x;
		yold = rect[i].y;
		rect[i].x = x + (xold - x) * _cos + (yold - y) * _sin;
		rect[i].y = y - (xold - x) * _sin + (yold - y) * _cos;
	}
}

void Sprite::draw() const
{
	if (!_empty)
	{
		auto tex = _texture.lock();
		if (tex && tex->isValid())
			RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATETEXTURERASTER, tex->getRwTexture()->raster);
		else
			return;
	}
	else
	{
		RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATETEXTURERASTER, nullptr);
	}
	if (std::abs(_angle) > 0.00001f)
	{
		CVector2D vertices[4];
		vertices[0].x = _rect.left; vertices[0].y = _rect.bottom;
		vertices[1].x = _rect.right; vertices[1].y = _rect.bottom;
		vertices[2].x = _rect.left; vertices[2].y = _rect.top;
		vertices[3].x = _rect.right; vertices[3].y = _rect.top;
		rotateVertices(vertices, 4, _rect.left + (_rect.right - _rect.left) / 2.0f, _rect.top +
			(_rect.bottom - _rect.top) / 2.0f, _angle);
		CSprite2d::DrawAnyRect(vertices[0].x, vertices[0].y, vertices[1].x, vertices[1].y, vertices[2].x, vertices[2].y,
							   vertices[3].x, vertices[3].y, _color1, _color2, _color3, _color4);
	}
	else
	{
		CSprite2d::DrawAnyRect(_rect.left, _rect.bottom, _rect.right, _rect.bottom, _rect.left,
							   _rect.top, _rect.right, _rect.top, _color1, _color2, _color3, _color4);
	}
}
