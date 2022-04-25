#pragma once

#include "BaseGUI.h"

class CGUIText : public CBaseGUI
{
public:
    CGUIText(u16 color, int x, int y);
    virtual ~CGUIText();

    CGLTextTexture m_Texture{ CGLTextTexture() };

    virtual Core::Vec2<i32> GetSize() { return Core::Vec2<i32>(m_Texture.Width, m_Texture.Height); }

    void CreateTextureA(
        u8 font,
        const std::string &str,
        int width = 0,
        TEXT_ALIGN_TYPE align = TS_LEFT,
        u16 flags = 0);

    void CreateTextureW(
        u8 font,
        const std::wstring &str,
        u8 cell = 30,
        int width = 0,
        TEXT_ALIGN_TYPE align = TS_LEFT,
        u16 flags = 0);

    virtual void Draw(bool checktrans = false);
    virtual bool Select();
};
