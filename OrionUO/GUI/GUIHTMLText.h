// MIT License
// Copyright (C) August 2016 Hotride

#pragma once

#include "BaseGUI.h"

class CGUIHTMLText : public CBaseGUI
{
public:
    u32 TextID = 0;
    u32 HTMLStartColor = 0;
    std::wstring Text = {};
    u8 Font = 0;
    TEXT_ALIGN_TYPE Align = TS_LEFT;
    u16 TextFlags = 0;
    int Width = 0;

    CGUIHTMLText(
        int index,
        u8 font,
        u16 color,
        int x,
        int y,
        int width = 0,
        TEXT_ALIGN_TYPE align = TS_LEFT,
        u16 textFlags = 0,
        int htmlStartColor = 0xFFFFFFFF);
    virtual ~CGUIHTMLText();

    CGLHTMLTextTexture m_Texture{ CGLHTMLTextTexture() };

    virtual Core::Vec2<i32> GetSize() { return Core::Vec2<i32>(m_Texture.Width, m_Texture.Height); }
    void CreateTexture(bool backgroundCanBeColored);

    virtual void Draw(bool checktrans = false);
    virtual bool Select();
};
