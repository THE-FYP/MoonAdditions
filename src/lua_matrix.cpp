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
#include "lua_matrix.h"
#include "lua_vector.h"
#include "game_sa/CMatrix.h"
#include "game_sa/CGeneral.h"
#include "game_sa/common.h"


namespace lua_matrix
{
	CMatrix* fromMemory(uintptr_t address)
	{
		return reinterpret_cast<CMatrix*>(address);
	}

	uintptr_t getRawPointer(const CMatrix& mat)
	{
		return reinterpret_cast<uintptr_t>(&mat);
	}

	void constructEmpty(CMatrix& mat)
	{
		memset(&mat, 0, sizeof(mat));
	}

	void setFromMatrix(CMatrix& mat, const CMatrix& m)
	{
		// mat = m;
		mat.right = m.right;
		mat.up = m.up;
		mat.at = m.up;
		mat.flags = m.flags;
	}

	void setFromVectors(CMatrix& mat, const CVector& right, const CVector& up, const CVector& at, const CVector& pos)
	{
		mat.right = right;
		mat.up = up;
		mat.at = at;
		mat.pos = pos;
	}

	void setFromValues(CMatrix& mat, float rightX, float rightY, float rightZ, float upX, float upY, float upZ, float atX, float atY, float atZ, float posX, float posY, float posZ)
	{
		mat.right.Set(rightX, rightY, rightZ);
		mat.up.Set(upX, upY, upZ);
		mat.at.Set(atX, atY, atZ);
		mat.pos.Set(posX, posY, posZ);
	}

	CMatrix multiply(const CMatrix& mat1, const CMatrix& mat2)
	{
		CMatrix mat{mat1};
		mat *= mat2;
		return mat;
	}

	void rotateAroundAxis(CMatrix& mat, float x, float y, float z, float angle, sol::optional<RwOpCombineType> combineOp)
	{
		RwV3d axis{x, y, z};
		RwMatrixRotate(reinterpret_cast<RwMatrix*>(&mat), &axis, angle, combineOp.value_or(rwCOMBINEPOSTCONCAT));
	}

	std::tuple<float, float, float> getAngles(const CMatrix& mat)
	{
		auto getAngle = [](float x, float y) {
			float a = CGeneral::GetATanOfXY(x, y) * 57.295776f - 90.0f;
			while (a < 0.0f) a += 360.0f;
			return a;
		};
		float x = getAngle(mat.right.z, sqrt(mat.right.x * mat.right.x + mat.right.y * mat.right.y));
		float y = getAngle(mat.up.z, sqrt(mat.up.x * mat.up.x + mat.up.y * mat.up.y));
		float z = getAngle(mat.right.x, mat.right.y);
		return {x, y, z};
	}

	std::tuple<float, float, float> getLocalOffset(const CMatrix& mat, float x, float y, float z)
	{
		CVector coords{x, y, z};
		CMatrix matrixInv;
		RwMatrixInvert(reinterpret_cast<RwMatrix*>(&matrixInv), reinterpret_cast<const RwMatrix*>(&mat));
		CVector result = Multiply3x3(matrixInv, VectorSub(coords, mat.pos));
		return {result.x, result.y, result.z};
	}

	void rotate(CMatrix& mat, float x, float y, float z)
	{
		CVector pos = mat.pos;
		mat.SetRotate(x, y, z);
		mat.pos = pos;
	}
	
	std::tuple<float, float, float> transformPoint(const CMatrix& mat, float x, float y, float z)
	{
		RwV3d offset{x, y, z}, result;
		RwV3dTransformPoint(&result, &offset, reinterpret_cast<const RwMatrix*>(&mat));
		return {result.x, result.y, result.z};
	}

	void rotateX(CMatrix& mat, float angle)
	{
		mat.SetRotateXOnly(angle * 0.0174532925f);
	}

	void rotateY(CMatrix& mat, float angle)
	{
		mat.SetRotateYOnly(angle * 0.0174532925f);
	}

	void rotateZ(CMatrix& mat, float angle)
	{
		mat.SetRotateZOnly(angle * 0.0174532925f);
	}

	void rotateByQuat(CMatrix& mat, const CQuaternion& quat)
	{
		mat.SetRotate(quat);
	}

	CMatrix getSlerped(const CMatrix& mat1, const CMatrix& mat2, float t)
	{
		CMatrix out{mat1};
		CQuaternion quat, quat1, quat2;
		quat1.Set(*reinterpret_cast<const RwMatrix*>(&mat1));
		quat2.Set(*reinterpret_cast<const RwMatrix*>(&mat2));
		quat.Slerp(quat1, quat2, t);
		out.SetRotate(quat);
		return out;
	}

	CMatrix fromQuat(const CQuaternion& quat)
	{
		CMatrix mat;
		mat.SetRotate(quat);
		return mat;
	}

	bool equals(const CMatrix& lhs, const CMatrix& rhs)
	{
		return lhs.right == rhs.right && lhs.up == rhs.up && lhs.at == rhs.at && lhs.pos == rhs.pos;
	}

	void initialize(sol::table& module)
	{
		module.new_usertype<CMatrix>("matrix",
									 "from_memory", sol::factories(&fromMemory),
									 "get_pointer", &getRawPointer,
									 "new", sol::initializers(&constructEmpty, &setFromMatrix, &setFromVectors, &setFromValues),
									 "set", sol::overload(&setFromMatrix, &setFromVectors, &setFromValues),
									 "from_quat", sol::factories(&fromQuat),
									 "set_rotation_from_quat", &rotateByQuat,
									 "rotate", &rotate,
									 "rotate_x", &rotateX,
									 "rotate_y", &rotateY,
									 "rotate_z", &rotateZ,
									 "rotate_around_axis", &rotateAroundAxis,
									 "get_coords_with_offset", &transformPoint,
									 "get_relative_coords", &getLocalOffset,
									 "get_angles", &getAngles,
									 "get_slerped", &getSlerped,
									 "right", &CMatrix::right,
									 "up", &CMatrix::up,
									 "at", &CMatrix::at,
									 "pos", &CMatrix::pos,
									 "flags", &CMatrix::flags,
									 sol::meta_method::multiplication, &multiply,
									 sol::meta_method::equal_to, &equals);
	}
}
