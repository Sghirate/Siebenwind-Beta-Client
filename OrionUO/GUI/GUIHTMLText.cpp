// MIT License
// Copyright (C) August 2016 Hotride

#include "GUIHTMLText.h"
#include "../Managers/FontsManager.h"
#include "../Managers/MouseManager.h"

CGUIHTMLText::CGUIHTMLText(
    int index,
    u8 font,
    u16 color,
    int x,
    int y,
    int width,
    TEXT_ALIGN_TYPE align,
    u16 textFlags,
    int htmlStartColor)
    : CBaseGUI(GOT_HTMLTEXT, 0, 0, color, x, y)
    , TextID(index)
    , HTMLStartColor(htmlStartColor)
    , Text({})
    , Font(font)
    , Align(align)
    , TextFlags(textFlags)
    , Width(width)
{
}

CGUIHTMLText::~CGUIHTMLText()
{
    m_Texture.Clear();
}

void CGUIHTMLText::CreateTexture(bool backgroundCanBeColored)
{
    g_FontManager.SetUseHTML(true, HTMLStartColor, backgroundCanBeColored);
    g_FontManager.GenerateW(Font, m_Texture, Text, Color, 30, Width, Align, TextFlags);
    g_FontManager.SetUseHTML(false);
}

void CGUIHTMLText::Draw(bool checktrans)
{
    m_Texture.Draw(m_X, m_Y, checktrans);
}

bool CGUIHTMLText::Select()
{
    Core::TMousePos pos = g_MouseManager.GetPosition();
    int x = pos.x - m_X;
    int y = pos.y - m_Y;
    return (x >= 0 && y >= 0 && x < m_Texture.Width && y < m_Texture.Height);
}
