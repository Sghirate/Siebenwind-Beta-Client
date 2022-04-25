#pragma once

#include "../plugin/mulstruct.h"

struct FLOAT_HUES
{
    float Palette[32 * 3];
};

class CColorManager
{
private:
    std::vector<HUES_GROUP> m_HuesRange;
    std::vector<FLOAT_HUES> m_HuesFloat;
    int m_HuesCount{ 0 };
    std::vector<u16> m_Radarcol;

public:
    CColorManager();
    ~CColorManager();

    void Init();

    HUES_GROUP *GetHuesRangePointer() { return &m_HuesRange[0]; }
    int GetHuesCount() const { return m_HuesCount; }
    void SetHuesBlock(int index, VERDATA_HUES_GROUP *group);
    void CreateHuesPalette();
    void SendColorsToShader(u16 color);
    u16 Color32To16(int c);
    u32 Color16To32(u16 c);
    u16 ConvertToGray(u16 c);
    u16 GetColor16(u16 c, u16 color);
    u16 GetRadarColorData(int c);
    u32 GetPolygoneColor(u16 c, u16 color);
    u32 GetUnicodeFontColor(u16 &c, u16 color);
    u32 GetColor(u16 &c, u16 color);
    u32 GetPartialHueColor(u16 &c, u16 color);
    u16 FixColor(u16 color, u16 defaultColor = 0);
};

extern CColorManager g_ColorManager;
