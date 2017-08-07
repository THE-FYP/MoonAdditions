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
#include "lua_quat.h"
#include "lua_vector.h"


namespace lua_quat
{
	void constructEmpty(CQuaternion& quat)
	{
		quat.imag.x = 0;
		quat.imag.y = 0;
		quat.imag.z = 0;
		quat.real = 0.0f;
	}

	void setFromVector(CQuaternion& quat, const CVector& vec, float w)
	{
		quat.imag = vec;
		quat.real = w;
	}

	void setFromMatrix(CQuaternion& quat, const CMatrix& mat)
	{
		quat.Set(*reinterpret_cast<const RwMatrix*>(&mat));
	}

	void setFromValues(CQuaternion& quat, float x, float y, float z, float w)
	{
		quat.imag.Set(x, y, z);
		quat.real = w;
	}

	void setFromQuaternion(CQuaternion& quat, const CQuaternion& r)
	{
		quat = r;
	}

	std::tuple<double, double, double, double> getValues(const CQuaternion& q)
	{
		return {q.imag.x, q.imag.y, q.imag.z, q.real};
	}

	CQuaternion* fromMemory(uintptr_t quat)
	{
		return reinterpret_cast<CQuaternion*>(quat);
	}

	uintptr_t getRawPointer(const CQuaternion& quat)
	{
		return reinterpret_cast<uintptr_t>(&quat);
	}

	void rotate(CQuaternion& quat, float x, float y, float z, float angle, sol::optional<RwOpCombineType> combineOp)
	{
		RwV3d axis{x, y, z};
		RtQuatRotate(reinterpret_cast<RtQuat*>(&quat), &axis, angle, combineOp.value_or(rwCOMBINEPOSTCONCAT));
	}

	CQuaternion getNormalized(const CQuaternion& quat)
	{
		CQuaternion q;
		q.Normalise();
		return q;
	}

	CQuaternion multiply(const CQuaternion& quat1, const CQuaternion& quat2)
	{
		CQuaternion q;
		q.Multiply(quat1, quat2);
		return q;
	}

	CQuaternion getSlerped(const CQuaternion& quat1, const CQuaternion& quat2, float t)
	{
		CQuaternion q;
		q.Slerp(quat1, quat2, t);
		return q;
	}

	CQuaternion fromEuler(float x, float y, float z)
	{
		CQuaternion q;
		q.Set(x, y, z);
		return q;
	}

	std::tuple<float, float, float> getEuler(CQuaternion& quat)
	{
		float x, y, z;
		quat.Get(&x, &y, &z);
		return {x, y, z};
	}

	bool equals(const CQuaternion& lhs, const CQuaternion& rhs)
	{
		return lhs.imag == rhs.imag && lhs.real == rhs.real;
	}

	void initialize(sol::table& module)
	{
		module.new_usertype<CQuaternion>("quaternion",
										 "from_memory", sol::factories(&fromMemory),
										 "get_pointer", &getRawPointer,
										 "new", sol::initializers(&constructEmpty, &setFromQuaternion, &setFromVector, &setFromValues, &setFromMatrix),
										 "set", sol::overload(&setFromQuaternion, &setFromVector, &setFromValues, &setFromMatrix),
										 "get", &getValues,
										 "rotate", &rotate,
										 "normalize", &CQuaternion::Normalise,
										 "get_normalized", &getNormalized,
										 "get_slerped", &getSlerped,
										 "get_euler", &getEuler,
										 "from_euler", sol::factories(&fromEuler),
										 "x", sol::property([](const CQuaternion& q) { return q.imag.x; }, [](CQuaternion& q, float v) { q.imag.x = v; }),
										 "y", sol::property([](const CQuaternion& q) { return q.imag.y; }, [](CQuaternion& q, float v) { q.imag.y = v; }),
										 "z", sol::property([](const CQuaternion& q) { return q.imag.z; }, [](CQuaternion& q, float v) { q.imag.z = v; }),
										 "w", &CQuaternion::real,
										 sol::meta_method::multiplication, &multiply,
										 sol::meta_method::equal_to, &equals);
		module.new_enum<false>("op_combine_type",
						"REPLACE", rwCOMBINEREPLACE,
						"PRE_CONCAT", rwCOMBINEPRECONCAT,
						"POST_CONCAT", rwCOMBINEPOSTCONCAT);
	}
}
