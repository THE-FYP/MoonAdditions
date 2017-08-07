/*
    Plugin-SDK (Grand Theft Auto) header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once
#include "plbase/PluginBase_SA.h"
#include "CAEAudioEntity.h"

class CAEWeatherAudioEntity : public CAEAudioEntity {
public:
    unsigned char m_nThunderFrequencyVariationCounter;
private:
    char _pad7D[3];
public:


};

VALIDATE_SIZE(CAEWeatherAudioEntity, 0x80);