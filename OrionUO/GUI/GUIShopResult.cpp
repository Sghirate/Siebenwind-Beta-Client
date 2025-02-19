#include "GUIShopResult.h"
#include "GUIMinMaxButtons.h"
#include "GUIShopItem.h"
#include "Globals.h"
#include "OrionUO.h"
#include "Managers/MouseManager.h"
#include "Managers/FontsManager.h"

CGUIShopResult::CGUIShopResult(CGUIShopItem* shopItem, int x, int y)
    : CBaseGUI(GOT_SHOPRESULT, shopItem->Serial, shopItem->Graphic, shopItem->Color, x, y)
    , Price(shopItem->Price)
    , Name(shopItem->Name)
{
    MoveOnDrag = true;

    std::string name = Name + "\n" + "at " + std::to_string(Price) + " g.p.";
    g_FontManager.GenerateA(9, m_NameText, name, 0x021F, 100);

    int maxCount = shopItem->Count;

    if (maxCount > 999)
    {
        maxCount = 999;
    }

    m_MinMaxButtons =
        new CGUIMinMaxButtons(Serial, 0x0037, 156, m_NameText.Height / 2, 0, maxCount, 1);
    m_MinMaxButtons->DefaultTextOffset = -122;
    m_MinMaxButtons->SetTextParameters(true, STP_LEFT_CENTER, 9, 0x021F, false);
}

CGUIShopResult::~CGUIShopResult()
{
    m_NameText.Clear();
    if (m_MinMaxButtons)
    {
        delete m_MinMaxButtons;
        m_MinMaxButtons = nullptr;
    }
}

CBaseGUI* CGUIShopResult::SelectedItem()
{
    CBaseGUI* result     = this;
    Core::Vec2<i32> size = m_MinMaxButtons->GetSize();

    if (g_Orion.PolygonePixelsInXY(
            m_X + m_MinMaxButtons->GetX(), m_Y + m_MinMaxButtons->GetY(), size.x, size.y))
    {
        result = m_MinMaxButtons;
    }

    return result;
}

void CGUIShopResult::PrepareTextures()
{
    m_MinMaxButtons->PrepareTextures();
}

void CGUIShopResult::Draw(bool checktrans)
{
    glTranslatef((GLfloat)m_X, (GLfloat)m_Y, 0.0f);

    glUniform1iARB(g_ShaderDrawMode, SDM_NO_COLOR);

    m_NameText.Draw(34, 0, checktrans);
    m_MinMaxButtons->Draw(checktrans);

    glTranslatef((GLfloat)-m_X, (GLfloat)-m_Y, 0.0f);
}

bool CGUIShopResult::Select()
{
    Core::TMousePos pos = g_MouseManager.GetPosition();
    int x               = pos.x - m_X;
    int y               = pos.y - m_Y;
    return (x >= 0 && y >= 0 && x < 200 && y < m_NameText.Height);
}
