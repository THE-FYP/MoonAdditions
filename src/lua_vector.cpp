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
#include "lua_vector.h"


namespace lua_vector
{
	std::tuple<float, float, float> getValues(const CVector& vec)
	{
		return {vec.x, vec.y, vec.z};
	}

	CVector* fromMemory(uintptr_t address)
	{
		return reinterpret_cast<CVector*>(address);
	}

	uintptr_t getRawPointer(const CVector& vec)
	{
		return reinterpret_cast<uintptr_t>(&vec);
	}

	CVector constructEmpty()
	{
		return CVector{0, 0, 0};
	}

	CVector constructFromValues(float x, float y, float z)
	{
		return CVector{x, y, z};
	}

	CVector constructFromVector(const CVector& vec)
	{
		return CVector{vec.x, vec.y, vec.z};
	}

	void initialize(sol::table& module)
	{
		module.new_usertype<CVector>("vector3d",
									 "new", sol::factories(&constructEmpty, &constructFromValues, &constructFromVector),
									 // #pluginsdk
									 // sol::constructors<CVector(), CVector(float, float, float), CVector(const CVector&)>(),
									 "from_memory", sol::factories(&fromMemory),
									 "get_pointer", &getRawPointer,
									 "x", &CVector::x,
									 "y", &CVector::y,
									 "z", &CVector::z,
									 "get", &getValues,
									 "set", &CVector::Set,
									 "magnitude", &CVector::Magnitude,
									 "magnitude2d", &CVector::Magnitude2D,
									 "normalize", &CVector::Normalise,
									 sol::meta_method::equal_to, static_cast<bool(*)(const CVector&, const CVector&)>(&operator==));
	}
}
