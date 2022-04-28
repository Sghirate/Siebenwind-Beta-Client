#pragma once

#include "../IndexObject.h"
#include "Core/Minimal.h"
#include <vector>

class UOFileReader
{
public:
    UOFileReader() {}
    ~UOFileReader() {}

    std::vector<u16> GetGumpPixels(CIndexObject& io);
    CGLTexture* ReadGump(CIndexObject& io);
    std::vector<u16>
    GetArtPixels(u16 id, CIndexObject& io, bool run, short& width, short& height);
    CGLTexture* ReadArt(u16 id, CIndexObject& io, bool run);
    CGLTexture* ReadTexture(CIndexObject& io);
    CGLTexture* ReadLight(CIndexObject& io);
    std::pair<CGLTexture*, Core::Vec2<i16>> ReadCursor(u16 a_id, CIndexObject& a_io, bool a_run);
};

extern UOFileReader g_UOFileReader;
