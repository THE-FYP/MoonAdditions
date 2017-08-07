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
#include "lua_bone.h"
#include "plugin_sa/plugin.h"
#include "game_sa/CPed.h"
#include "game_sa/common.h"
#include "game_sa/ePedBones.h"


namespace lua_bone
{
	class Bone
	{
	public:
		Bone(CPed* ped, AnimBlendFrameData* data) :
			_ped(ped),
			_boneData(data)
		{
		}

		uintptr_t getRawPointer() const { return reinterpret_cast<uintptr_t>(_boneData); }
		CMatrix* getMatrix() const;
		CVector* getOffsetVector() const { return &_boneData->m_vOffset; }
		CQuaternion* getQuaternion() const;

	private:
		AnimBlendFrameData* _boneData;
		CPed* _ped;
	};

	CMatrix* Bone::getMatrix() const
	{
		_ped->UpdateRpHAnim();
		RpHAnimHierarchy* hierarchy = GetAnimHierarchyFromSkinClump(_ped->m_pRwClump);
		if (!hierarchy)
		{
			return static_cast<CMatrix*>(_ped->GetMatrix());
		}
		else
		{
			auto index = RpHAnimIDGetIndex(hierarchy, _boneData->m_dwNodeId);
			return reinterpret_cast<CMatrix*>(&hierarchy->pMatrixArray[index]);
		}
	}

	CQuaternion* Bone::getQuaternion() const
	{
		// #pluginsdk
		/* struct RpHAnimBlendInterpFrame
			RtQuat quaternion;
			RwV3d translation;
		*/
		return reinterpret_cast<CQuaternion*>(&_boneData->m_pIFrame);
	}

	sol::optional<Bone> getBone(unsigned int pedHandle, sol::object bone)
	{
		std::string boneName;
		bool useName = bone.is<std::string>();
		if (useName)
			boneName = bone.as<std::string>();
		CPed* ped = CPools::GetPed(pedHandle);
		if (!ped || !ped->m_pRwObject || (useName && boneName.empty()))
			return sol::nullopt;
		AnimBlendFrameData* bonePtr;
		if (useName)
		{
			bonePtr = RpAnimBlendClumpFindFrame(ped->m_pRwClump, boneName.c_str());
		}
		else
		{
			unsigned int boneId = bone.as<unsigned int>();
			bonePtr = RpAnimBlendClumpFindBone(ped->m_pRwClump, boneId);
		}
		if (!bonePtr)
			return sol::nullopt;
		return Bone{ped, bonePtr};
	}

	void initialize(sol::table& module)
	{
		module.set_function("get_char_bone", &getBone);
		module.new_usertype<Bone>("bone",
								  "new", sol::no_constructor,
								  "get_pointer", &Bone::getRawPointer,
								  "matrix", sol::property(&Bone::getMatrix),
								  "offset", sol::property(&Bone::getOffsetVector),
								  "quaternion", sol::property(&Bone::getQuaternion));
		module.new_enum<false>("bone_id",
						"PELVIS_1", BONE_PELVIS1,
						"PELVIS", BONE_PELVIS,
						"SPINE", BONE_SPINE1,
						"UPPER_TORSO", BONE_UPPERTORSO,
						"NECK", BONE_NECK,
						"HEAD_2", BONE_HEAD2,
						"HEAD_1", BONE_HEAD1,
						"HEAD", BONE_HEAD,
						"RIGHT_UPPER_TORSO", BONE_RIGHTUPPERTORSO,
						"RIGHT_SHOULDER", BONE_RIGHTSHOULDER,
						"RIGHT_ELBOW", BONE_RIGHTELBOW,
						"RIGHT_WRIST", BONE_RIGHTWRIST,
						"RIGHT_HAND", BONE_RIGHTHAND,
						"RIGHT_THUMB", BONE_RIGHTTHUMB,
						"LEFT_UPPER_TORSO", BONE_LEFTUPPERTORSO,
						"LEFT_SHOULDER", BONE_LEFTSHOULDER,
						"LEFT_ELBOW", BONE_LEFTELBOW,
						"LEFT_WRIST", BONE_LEFTWRIST,
						"LEFT_HAND", BONE_LEFTHAND,
						"LEFT_THUMB", BONE_LEFTTHUMB,
						"LEFT_HIP", BONE_LEFTHIP,
						"LEFT_KNEE", BONE_LEFTKNEE,
						"LEFT_ANKLE", BONE_LEFTANKLE,
						"LEFT_FOOT", BONE_LEFTFOOT,
						"RIGHT_HIP", BONE_RIGHTHIP,
						"RIGHT_KNEE", BONE_RIGHTKNEE,
						"RIGHT_ANKLE", BONE_RIGHTANKLE,
						"RIGHT_FOOT", BONE_RIGHTFOOT);
	}
}
