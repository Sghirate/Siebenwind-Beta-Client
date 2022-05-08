#include "GUIText.h"
#include "Managers/FontsManager.h"
#include "Managers/MouseManager.h"
#include "SiebenwindClient.h"

CGUIText::CGUIText(u16 color, int x, int y)
    : CBaseGUI(GOT_TEXT, 0, 0, color, x, y)
{
}

CGUIText::~CGUIText()
{
    m_Texture.Clear();
}

void CGUIText::CreateTextureA(
    u8 font, const std::string& str, int width, TEXT_ALIGN_TYPE align, u16 flags)
{
    g_FontManager.GenerateA(
        font, m_Texture, SiebenwindClient::LocalizeA(str), Color, width, align, flags);
}

void CGUIText::CreateTextureW(
    u8 font, const std::wstring& str, u8 cell, int width, TEXT_ALIGN_TYPE align, u16 flags)
{
    g_FontManager.GenerateW(font, m_Texture, str, Color, cell, width, align, flags);
}

void CGUIText::Draw(bool checktrans)
{
    m_Texture.Draw(m_X, m_Y, checktrans);
}

bool CGUIText::Select()
{
    Core::TMousePos pos = g_MouseManager.GetPosition();
    int x               = pos.x - m_X;
    int y               = pos.y - m_Y;
    return (x >= 0 && y >= 0 && x < m_Texture.Width && y < m_Texture.Height);
}
