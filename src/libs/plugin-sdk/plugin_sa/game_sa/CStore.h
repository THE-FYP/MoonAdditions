/*
    Plugin-SDK (Grand Theft Auto) header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once
#include "plbase/PluginBase_SA.h"

template <typename ObjectType, unsigned int Capacity>
class CStore {
public:
    unsigned int count;
    ObjectType   objects[Capacity];

    CStore() {
        count = 0;
    }
};