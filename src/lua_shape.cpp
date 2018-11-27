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
#include "lua_shape.h"
#include "lua_texture.h"
#include "shape_drawer.h"
#include "game_sa/RenderWare.h"


namespace lua_shape
{
	Shape::Vertex::Vertex(float vx, float vy, CRGBA color, float vu, float vv)
	{
		x = vx, y = vy, z = 1.0f;
		rhw = 1.0f;
		u = vu, v = vv;
		setColor(color);
	}

	void Shape::Vertex::invertXY(bool invertX, bool invertY)
	{
		if (invertX)
		{
			x = RsGlobal.maximumWidth - x;
			u = 1.0f - u;
		}
		if (invertY)
		{
			y = RsGlobal.maximumHeight - y;
			v = 1.0f - v;
		}
	}

	void Shape::Vertex::rotate2d(float aroundX, float aroundY, float angle)
	{
		float a = angle / 57.2957795f;
		float _sin = sin(a), _cos = cos(a);
		x = aroundX + (x - aroundX) * _cos + (y - aroundY) * _sin;
		y = aroundY - (x - aroundX) * _sin + (y - aroundY) * _cos;
	}

	void Shape::Vertex::setColor(CRGBA color)
	{
		emissiveColor = color.ToIntARGB();
	}

	CRGBA Shape::Vertex::getColor() const
	{
		auto color = reinterpret_cast<const byte*>(&emissiveColor);
		return CRGBA{color[2], color[1], color[0], color[3]};
	}

	void Shape::draw(RwPrimitiveType type, bool enableVertexAlpha, unsigned int srcBlend, unsigned int dstBlend, opt<std::shared_ptr<lua_texture::Texture>> texture) const
	{
		ShapeDrawer::get()->add(shared_from_this(), texture ? texture.value() : nullptr, type, enableVertexAlpha, srcBlend, dstBlend);
	}

	void Shape::addVertex(float x, float y, CRGBA color, opt<float> u, opt<float> v, opt<bool> invertX, opt<bool> invertY)
	{
		_vertices.emplace_back(x, y, color, u.value_or(0.0f), v.value_or(0.0f));
		if (invertX)
			_vertices.back().invertXY(invertX.value_or(false), invertY.value_or(false));
	}

	bool Shape::setVertex(size_t index, float x, float y, CRGBA color, opt<float> u, opt<float> v, opt<bool> invertX, opt<bool> invertY)
	{
		if (index == 0 || index > _vertices.size() + 1)
			return false;
		if (index > _vertices.size())
			return addVertex(x, y, color, u, v, invertX, invertY), true;
		Vertex& vert = _vertices[index - 1];
		vert = Vertex{x, y, color, u.value_or(0.0f), v.value_or(0.0f)};
		if (invertX)
			vert.invertXY(invertX.value_or(false), invertY.value_or(false));
		return true;
	}

	bool Shape::setVertexFromVertex(size_t index, const Vertex& vert)
	{
		if (index == 0 || index > _vertices.size() + 1)
			return false;
		if (index > _vertices.size())
		{
			_vertices.push_back(vert);
			return true;
		}
		_vertices[index - 1] = vert;
		return true;
	}

	void Shape::clearVertices()
	{
		_vertices.clear();
	}

	size_t Shape::getVerticesNumber() const
	{
		return _vertices.size();
	}

	sol::optional<Shape::Vertex&> Shape::getVertex(size_t index)
	{
		if (index == 0 || index > _vertices.size())
			return sol::nullopt;
		return _vertices[index - 1];
	}

	bool Shape::rotateVertex(size_t index, float aroundX, float aroundY, float angle)
	{
		if (index == 0 || index > _vertices.size())
			return false;
		Vertex& vert = _vertices[index - 1];
		vert.rotate2d(aroundX, aroundY, angle);
		return true;
	}

	bool Shape::rotateVerticesRange(size_t from, size_t to, float aroundX, float aroundY, float angle)
	{
		if (from <= to && from > 0 && from <= _vertices.size() && to <= _vertices.size())
		{
			for (; from <= to; ++from)
				_vertices[from - 1].rotate2d(aroundX, aroundY, angle);
			return true;
		}
		return false;
	}

	bool Shape::rotateAllVertices(float aroundX, float aroundY, float angle)
	{
		if (_vertices.empty())
			return false;
		for (auto& v : _vertices)
			v.rotate2d(aroundX, aroundY, angle);
		return true;
	}

	std::shared_ptr<Shape> Shape::create()
	{
		return std::make_shared<Shape>();
	}

	void initialize(sol::table& module)
	{
		module.new_usertype<Shape::Vertex>("vertex", "new", sol::no_constructor,
										   "x", &Shape::Vertex::x,
										   "y", &Shape::Vertex::y,
										   "z", &Shape::Vertex::z,
										   "u", &Shape::Vertex::u,
										   "v", &Shape::Vertex::v,
										   "rhw", &Shape::Vertex::rhw,
										   "set_color", &Shape::Vertex::setColor,
										   "get_color", &Shape::Vertex::getColor);
		module.new_usertype<Shape>("shape",
								   "new", sol::factories(&Shape::create),
								   "draw", &Shape::draw,
								   "add_vertex", &Shape::addVertex,
								   "set_vertex", sol::overload(&Shape::setVertex, &Shape::setVertexFromVertex),
								   "clear", &Shape::clearVertices,
								   "vertices_number", sol::property(&Shape::getVerticesNumber),
								   "rotate_vertex", &Shape::rotateVertex,
								   "rotate_vertices_range", &Shape::rotateVerticesRange,
								   "rotate_all_vertices", &Shape::rotateAllVertices,
								   "get_vertex", &Shape::getVertex);
		module.new_enum<false>("primitive_type",
						"LINELIST", rwPRIMTYPELINELIST,
						"POLYLINE", rwPRIMTYPEPOLYLINE,
						"TRIANGLELIST", rwPRIMTYPETRILIST,
						"TRIANGLESTRIP", rwPRIMTYPETRISTRIP,
						"TRIANGLEFAN", rwPRIMTYPETRIFAN,
						"POINTLIST", rwPRIMTYPEPOINTLIST);
		module.new_enum<false>("blend_method",
							   "ZERO", rwBLENDZERO,
							   "ONE", rwBLENDONE,
							   "SRCCOLOR", rwBLENDSRCCOLOR,
							   "INVSRCCOLOR", rwBLENDINVSRCCOLOR,
							   "SRCALPHA", rwBLENDSRCALPHA,
							   "INVSRCALPHA", rwBLENDINVSRCALPHA,
							   "DESTALPHA", rwBLENDDESTALPHA,
							   "INVDESTALPHA", rwBLENDINVDESTALPHA,
							   "DESTCOLOR", rwBLENDDESTCOLOR,
							   "INVDESTCOLOR", rwBLENDINVDESTCOLOR,
							   "SRCALPHASAT", rwBLENDSRCALPHASAT);
	}
}
