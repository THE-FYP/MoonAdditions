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
#include "drawer.h"
#include "singleton.h"
#include "forward_declarations.h"


class Shape
{
public:
	Shape(std::shared_ptr<const lua_shape::Shape> shape, std::shared_ptr<const lua_texture::Texture> texture, RwPrimitiveType type, bool enableAlpha, byte srcBlend, byte dstBlend) :
		_shape(shape),
		_texture(texture),
		_primType(type),
		_enableAlpha(enableAlpha),
		_srcBlend(srcBlend),
		_dstBlend(dstBlend)
	{
	}
	
	void draw() const;

private:
	std::weak_ptr<const lua_shape::Shape> _shape;
	std::weak_ptr<const lua_texture::Texture> _texture;
	RwPrimitiveType _primType;
	bool _enableAlpha;
	byte _srcBlend;
	byte _dstBlend;
};

class ShapeDrawer : private Drawer<Shape>, public Singleton<ShapeDrawer>
{
public:
	void drawShapes();
	using Drawer<Shape>::add;

private:
	struct RenderStateData
	{
		void* _TEXTURERASTER;
		void* _VERTEXALPHAENABLE;
		void* _SRCBLEND;
		void* _DESTBLEND;
	};

	void storeRenderStates(RenderStateData& rsData) const;
	void restoreRenderStates(const RenderStateData& rsData);
};
