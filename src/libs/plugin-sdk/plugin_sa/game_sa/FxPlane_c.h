/*
    Plugin-SDK (Grand Theft Auto) header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "plbase/PluginBase_SA.h"
#include "RenderWare.h"

class FxPlane_c {
public:
    RwV3d m_vecNormal;
    float m_fDistance;
};

VALIDATE_SIZE(FxPlane_c, 0x10);