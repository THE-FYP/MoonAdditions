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
#include "forward_declarations.h"
#include "optional.h"


namespace lua_shape
{
	using namespace optional;

	class Shape : public std::enable_shared_from_this<Shape>
	{
	public:
		struct Vertex : public RwIm2DVertex
		{
			Vertex(float vx, float vy, CRGBA color, float vu, float vv);

			void invertXY(bool invertX, bool invertY);
			void rotate2d(float aroundX, float aroundY, float angle);
			void setColor(CRGBA color);
			CRGBA getColor() const;
		};

	public:
		void draw(RwPrimitiveType type, bool enableVertexAlpha, unsigned int srcBlend, unsigned int dstBlend, opt<std::shared_ptr<lua_texture::Texture>> texture) const;
		void addVertex(float x, float y, CRGBA color, opt<float> u, opt<float> v, opt<bool> invertX, opt<bool> invertY);
		bool setVertex(size_t index, float x, float y, CRGBA color, opt<float> u, opt<float> v, opt<bool> invertX, opt<bool> invertY);
		bool setVertexFromVertex(size_t index, const Vertex& vert);
		void clearVertices();
		size_t getVerticesNumber() const;
		sol::optional<Shape::Vertex&> getVertex(size_t index);
		bool rotateVertex(size_t index, float aroundX, float aroundY, float angle);
		bool rotateVerticesRange(size_t from, size_t to, float aroundX, float aroundY, float angle);
		bool rotateAllVertices(float aroundX, float aroundY, float angle);
		auto& getVertices() const { return _vertices; }
		
		static std::shared_ptr<Shape> create();

	private:
		std::vector<Vertex> _vertices;
	};

	void initialize(sol::table& module);
}
