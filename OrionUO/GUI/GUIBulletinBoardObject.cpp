#include "GUIBulletinBoardObject.h"
#include "Core/StringUtils.h"
#include "../Config.h"
#include "../OrionUO.h"
#include "../Managers/FontsManager.h"
#include "../Managers/MouseManager.h"

CGUIBulletinBoardObject::CGUIBulletinBoardObject(int serial, int x, int y, const std::wstring &text)
    : CBaseGUI(GOT_BB_OBJECT, serial, 0, 0, x, y)
    , Text(text)
{
    MoveOnDrag = true;
    if (g_Config.ClientVersion >= CV_305D)
        g_FontManager.GenerateW(1, m_Texture, text, 0);
    else
        g_FontManager.GenerateA(9, m_Texture, Core::ToString(text), 0x0386);
}

CGUIBulletinBoardObject::~CGUIBulletinBoardObject()
{
    m_Texture.Clear();
}

void CGUIBulletinBoardObject::PrepareTextures()
{
    g_Orion.ExecuteGump(0x1523);
}

void CGUIBulletinBoardObject::Draw(bool checktrans)
{
    CGLTexture *th = g_Orion.ExecuteGump(0x1523);
    if (th != nullptr)
    {
        th->Draw(m_X, m_Y, checktrans);
    }
    m_Texture.Draw(m_X + 23, m_Y + 1);
}

bool CGUIBulletinBoardObject::Select()
{
    Core::TMousePos pos = g_MouseManager.GetPosition();
    int x = pos.x - m_X;
    int y = pos.y - m_Y;
    return (x >= 0 && y >= 0 && x < 230 && y < 18);
}
