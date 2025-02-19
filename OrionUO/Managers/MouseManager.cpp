#include "MouseManager.h"
#include "ColorManager.h"
#include "Globals.h"
#include "MapManager.h"
#include "ConfigManager.h"
#include "CustomHousesManager.h"
#include "ScreenEffectManager.h"
#include "ScreenStages/BaseScreen.h"
#include "OrionUO.h"
#include "ToolTip.h"
#include "Target.h"
#include "PressedObject.h"
#include "Profiler.h"
#include "SelectedObject.h"
#include "GameObjects/ObjectOnCursor.h"
#include "GameObjects/GamePlayer.h"
#include "Gumps/GumpCustomHouse.h"
#include "Walker/PathFinder.h"

namespace
{
static struct CursorData
{
    u16 graphicDefault;
    u16 graphicCombat;
    CGLTexture* thDefault;
    CGLTexture* thCombat;
    Core::Vec2<i16> offset;
} g_cursors[16] = {
    // clang-format off
    { 0x206A, 0x2053, {} }, // N
    { 0x206B, 0x2054, {} }, // NE
    { 0x206C, 0x2055, {} }, // E
    { 0x206D, 0x2056, {} }, // SE
    { 0x206E, 0x2057, {} }, // S
    { 0x206F, 0x2058, {} }, // SW
    { 0x2070, 0x2059, {} }, // W
    { 0x2071, 0x205A, {} }, // NW
    { 0x2072, 0x205B, {} }, // Fist
    { 0x2073, 0x205C, {} }, // Default
    { 0x2074, 0x205D, {} }, // Pick
    { 0x2075, 0x205E, {} }, // Open
    { 0x2076, 0x205F, {} }, // Target
    { 0x2077, 0x2060, {} }, // Wait
    { 0x2078, 0x2061, {} }, // Write
    { 0x2079, 0x2062, {} }, // Pin
    // clang-format on
};
} // namespace

MouseManager g_MouseManager;

int MouseManager::Sgn(int val) const
{
    return static_cast<int>(0 < val) - static_cast<int>(val < 0);
}

int MouseManager::GetFacing(int x1, int y1, int to_x, int to_y, int current_facing) const
{
    int shiftX = to_x - x1;
    int shiftY = to_y - y1;
    int hashf  = 100 * (Sgn(shiftX) + 2) + 10 * (Sgn(shiftY) + 2);
    if ((shiftX != 0) && (shiftY != 0))
    {
        shiftX = std::abs(shiftX);
        shiftY = std::abs(shiftY);

        if ((shiftY * 5) <= (shiftX * 2))
        {
            hashf = hashf + 1;
        }
        else if ((shiftY * 2) >= (shiftX * 5))
        {
            hashf = hashf + 3;
        }
        else
        {
            hashf = hashf + 2;
        }
    }
    else if (shiftX == 0)
    {
        if (shiftY == 0)
        {
            return current_facing;
        }
    }
    switch (hashf)
    {
        case 111: return DT_W;  // W
        case 112: return DT_NW; // NW
        case 113: return DT_N;  // N
        case 120: return DT_W;  // W
        case 131: return DT_W;  // W
        case 132: return DT_SW; // SW
        case 133: return DT_S;  // S
        case 210: return DT_N;  // N
        case 230: return DT_S;  // S
        case 311: return DT_E;  // E
        case 312: return DT_NE; // NE
        case 313: return DT_N;  // N
        case 320: return DT_E;  // E
        case 331: return DT_E;  // E
        case 332: return DT_SE; // SE
        case 333: return DT_S;  // S
        default: break;
    }

    return current_facing;
}

MouseCursors MouseManager::DetermineCursor() const
{
    if (g_Target.IsTargeting() && !g_ObjectInHand.Enabled)
        return MouseCursors::Target; // target cursor

    Core::TMousePos pos = GetPosition();
    bool mouseInWindow =
        !(pos.x < g_ConfigManager.GameWindowX || pos.y < g_ConfigManager.GameWindowY ||
          pos.x > (g_ConfigManager.GameWindowX + g_ConfigManager.GameWindowWidth) ||
          pos.y > (g_ConfigManager.GameWindowY + g_ConfigManager.GameWindowHeight));

    //bool gumpChecked = (g_LastSelectedGump || (g_LastSelectedObject && g_LastObjectType != SOT_GAME_OBJECT && g_LastObjectType != SOT_STATIC_OBJECT && g_LastObjectType != SOT_LAND_OBJECT && g_LastObjectType != SOT_TEXT_OBJECT));

    if (!mouseInWindow || g_SelectedObject.Gump != nullptr || g_PressedObject.LeftGump != nullptr)
        return MouseCursors::Default; // default

    int gameWindowCenterX = g_ConfigManager.GameWindowX + (g_ConfigManager.GameWindowWidth / 2);
    int gameWindowCenterY = g_ConfigManager.GameWindowY + (g_ConfigManager.GameWindowHeight / 2);
    return (MouseCursors)GetFacing(gameWindowCenterX, gameWindowCenterY, pos.x, pos.y, 1);
}

void MouseManager::ProcessWalking()
{
    Core::TMousePos pos = GetPosition();
    bool mouseInWindow  = true;
    if (pos.x < g_ConfigManager.GameWindowX || pos.y < g_ConfigManager.GameWindowY ||
        pos.x > (g_ConfigManager.GameWindowX + g_ConfigManager.GameWindowWidth) ||
        pos.y > (g_ConfigManager.GameWindowY + g_ConfigManager.GameWindowHeight))
        mouseInWindow = false;

    if ((g_MovingFromMouse || (mouseInWindow && g_AutoMoving)) &&
        g_PressedObject.RightGump == nullptr &&
        !((g_ShiftPressed && !g_CtrlPressed && !g_AltPressed) &&
          g_ConfigManager.HoldShiftForEnablePathfind && g_ConfigManager.EnablePathfind) &&
        !(g_SelectedGameObjectHandle == g_PressedObject.RightSerial &&
          g_PressedObject.RightObject != nullptr && g_PressedObject.RightObject->IsGameObject()))
    {
        int gameWindowCenterX = g_ConfigManager.GameWindowX + (g_ConfigManager.GameWindowWidth / 2);
        int gameWindowCenterY =
            g_ConfigManager.GameWindowY + (g_ConfigManager.GameWindowHeight / 2);

        int facing =
            GetFacing(gameWindowCenterX, gameWindowCenterY, GetPosition().x, GetPosition().y, 1);

        float mouse_range = std::hypotf(
            (float)(gameWindowCenterX - GetPosition().x),
            (float)(gameWindowCenterY - GetPosition().y));

        int dir = facing;

        if (dir == 0)
        {
            dir = 8;
        }

        bool run = (mouse_range >= 190.0f);

        if (!g_PathFinder.AutoWalking)
        {
            g_PathFinder.Walk(run, dir - 1);
        }
    }
}

bool MouseManager::LoadCursorTextures()
{
    bool result = true;
    for (CursorData& cursorData : g_cursors)
    {
        auto defaultCursor   = g_Orion.ExecuteCursor(cursorData.graphicDefault);
        auto combatCursor    = g_Orion.ExecuteCursor(cursorData.graphicCombat);
        cursorData.thDefault = defaultCursor.first;
        cursorData.thCombat  = combatCursor.first;
        cursorData.offset    = defaultCursor.second;
    }
    return result;
}

void MouseManager::Draw(MouseCursors a_cursor)
{
    MouseCursors cursor =
        (int)a_cursor >= 0 && a_cursor < MouseCursors::COUNT ? a_cursor : DetermineCursor();
    PROFILER_EVENT();
    if (g_GameState >= GS_GAME)
    {
        if (g_CustomHouseGump != nullptr && (g_CustomHouseGump->SelectedGraphic != 0u))
        {
            u16 color = 0;

            std::vector<CBuildObject> list;
            CUSTOM_HOUSE_BUILD_TYPE type;

            if (!g_CustomHouseGump->CanBuildHere(
                    list, (CRenderWorldObject*)g_SelectedObject.Object, type))
            {
                color = 0x0021;
            }

            if (color != 0)
            {
                g_ColorizerShader.Use();
            }

            if (static_cast<unsigned int>(!list.empty()) != 0u)
            {
                for (const CBuildObject& item : list)
                {
                    int x = g_MouseManager.GetPosition().x + (item.X - item.Y) * 22;
                    int y = g_MouseManager.GetPosition().y + (item.X + item.Y) * 22 - (item.Z * 4);

                    g_Orion.DrawStaticArt(item.Graphic, color, x, y, false);
                }
            }
            else
            {
                g_Orion.DrawStaticArtInContainer(
                    g_CustomHouseGump->SelectedGraphic,
                    color,
                    g_MouseManager.GetPosition().x,
                    g_MouseManager.GetPosition().y,
                    false,
                    true);
            }

            if (color != 0)
            {
                UnuseShader();
            }
        }
        else if (g_ObjectInHand.Enabled)
        {
            bool doubleDraw = false;
            u16 ohGraphic   = g_ObjectInHand.GetDrawGraphic(doubleDraw);

            u16 ohColor = g_ObjectInHand.Color;
            doubleDraw =
                ((CGameObject::IsGold(g_ObjectInHand.Graphic) == 0) &&
                 IsStackable(g_ObjectInHand.TiledataPtr->Flags) && g_ObjectInHand.Count > 1);

            if (ohColor != 0)
            {
                g_ColorizerShader.Use();
            }

            if (g_ObjectInHand.IsGameFigure)
            {
                ohGraphic -= GAME_FIGURE_GUMP_OFFSET;

                CGLTexture* to = g_Orion.ExecuteGump(ohGraphic);

                if (to != nullptr)
                {
                    g_Orion.DrawGump(
                        ohGraphic,
                        ohColor,
                        g_MouseManager.GetPosition().x - (to->Width / 2),
                        g_MouseManager.GetPosition().y - (to->Height / 2));
                }
            }
            else
            {
                g_Orion.DrawStaticArtInContainer(
                    ohGraphic,
                    ohColor,
                    g_MouseManager.GetPosition().x,
                    g_MouseManager.GetPosition().y,
                    false,
                    true);

                if (doubleDraw)
                {
                    g_Orion.DrawStaticArtInContainer(
                        ohGraphic,
                        ohColor,
                        g_MouseManager.GetPosition().x + 5,
                        g_MouseManager.GetPosition().y + 5,
                        false,
                        true);
                }
            }

            if (ohColor != 0)
            {
                UnuseShader();
            }
        }
    }

    int war   = (int)(g_Player != nullptr && g_Player->Warmode);
    u16 color = (!war) && g_GameState >= GS_GAME && (g_MapManager.GetActualMap() != 0) ? 0x0033 : 0;
    CGLTexture* th     = war ? g_cursors[(int)cursor].thCombat : g_cursors[(int)cursor].thDefault;
    const auto& offset = g_cursors[(int)cursor].offset;
    if (th != nullptr)
    {
        g_ToolTip.Draw(th->Width, th->Height);

        int x = GetPosition().x - offset.x;
        int y = GetPosition().y - offset.y;

        if (color != 0u)
        {
            g_ColorizerShader.Use();

            g_ColorManager.SendColorsToShader(color);

            glUniform1iARB(g_ShaderDrawMode, SDM_COLORED);
        }

        th->Draw(x, y);

        if (color != 0u)
        {
            UnuseShader();
        }

        if (g_Target.Targeting && g_ConfigManager.HighlightTargetByType)
        {
            u32 auraColor = 0;

            if (g_Target.CursorType == 0)
            {
                auraColor = g_ColorManager.GetPolygoneColor(16, 0x03B2);
            }
            else if (g_Target.CursorType == 1)
            {
                auraColor = g_ColorManager.GetPolygoneColor(16, 0x0023);
            }
            else if (g_Target.CursorType == 2)
            {
                auraColor = g_ColorManager.GetPolygoneColor(16, 0x005A);
            }

            if (auraColor != 0u)
            {
                glEnable(GL_BLEND);
                glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

                glColor4ub(ToColorR(auraColor), ToColorG(auraColor), ToColorB(auraColor), 0xFF);

                glUniform1iARB(g_ShaderDrawMode, SDM_NO_COLOR);

                CGLTexture tex;
                tex.Texture = g_AuraTexture.Texture;
                tex.Width   = 35;
                tex.Height  = 35;

                g_GL.GL1_Draw(tex, x - 6, y - 2);

                tex.Texture = 0;

                glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
                glDisable(GL_BLEND);
            }
        }
    }
}

void MouseManager::EmulateOnLeftMouseButtonDown()
{
    if (g_CurrentScreen != nullptr && g_ScreenEffectManager.Mode == SEM_NONE)
    {
        g_CurrentScreen->SelectObject();
        g_PressedObject.InitLeft(g_SelectedObject);
        if (g_SelectedObject.Object != nullptr || g_GameState == GS_GAME)
        {
            LeftDropPosition = GetPosition();
            g_CurrentScreen->OnLeftMouseButtonDown();
        }
    }
}
