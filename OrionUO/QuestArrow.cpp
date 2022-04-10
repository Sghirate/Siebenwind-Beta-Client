#include "QuestArrow.h"
#include "OrionUO.h"
#include "Managers/MouseManager.h"
#include "ScreenStages/MainScreen.h"
#include "ScreenStages/GameScreen.h"
#include "GameObjects/GamePlayer.h"

CQuestArrow g_QuestArrow;

CQuestArrow::CQuestArrow()
{
}

CQuestArrow::~CQuestArrow()
{
}

void CQuestArrow::Draw()
{
    if (Enabled)
    {
        int dir = g_MouseManager.GetFacing(g_Player->GetX(), g_Player->GetY(), X, Y, 0);

        u16 gumpID = m_Gump + ((dir + 1) % 8);

        Core::Vec2<i32> size = g_Orion.GetGumpDimension(gumpID);

        int gox = X - g_Player->GetX();
        int goy = Y - g_Player->GetY();

        int x = g_RenderBounds.GameWindowCenterX + ((gox - goy) * 22) - (size.x / 2);
        int y = g_RenderBounds.GameWindowCenterY + ((gox + goy) * 22) - (size.y / 2);

        if (x < g_RenderBounds.GameWindowPosX)
        {
            x = g_RenderBounds.GameWindowPosX;
        }
        else if (x > g_RenderBounds.GameWindowPosX + g_RenderBounds.GameWindowWidth - size.x)
        {
            x = g_RenderBounds.GameWindowPosX + g_RenderBounds.GameWindowWidth - size.x;
        }

        if (y < g_RenderBounds.GameWindowPosY)
        {
            y = g_RenderBounds.GameWindowPosY;
        }
        else if (y > g_RenderBounds.GameWindowPosY + g_RenderBounds.GameWindowHeight - size.y)
        {
            y = g_RenderBounds.GameWindowPosY + g_RenderBounds.GameWindowHeight - size.y;
        }

        if (Timer < g_Ticks)
        {
            if (Timer + 120 < g_Ticks)
            {
                Timer = g_Ticks + 1000;
            }

            g_ColorizerShader.Use();

            g_Orion.DrawGump(gumpID, 0x0021, x, y);

            UnuseShader();
        }
        else
        {
            g_Orion.DrawGump(gumpID, 0, x, y);
        }
    }
}
