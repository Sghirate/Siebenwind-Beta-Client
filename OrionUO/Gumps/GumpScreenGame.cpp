#include "GumpScreenGame.h"
#include "Core/Platform.h"
#include "GameVars.h"
#include "GameWindow.h"
#include "GumpConsoleType.h"
#include "GumpOptions.h"
#include "Config.h"
#include "OrionUO.h"
#include "ToolTip.h"
#include "PressedObject.h"
#include "SelectedObject.h"
#include "Managers/GumpManager.h"
#include "Managers/ConfigManager.h"
#include "Managers/MouseManager.h"
#include "ScreenStages/GameScreen.h"
#include "Network/Packets.h"

CGumpScreenGame::CGumpScreenGame()
    : CGump(GT_NONE, 0, 0, 0)
{
    NoMove  = true;
    NoClose = true;

    Add(new CGUIButton(ID_GS_RESIZE, 0x0837, 0x0837, 0x0838, 0, 0));
    Add(new CGUIGumppic(0x0E14, 0, 0));
}

CGumpScreenGame::~CGumpScreenGame()
{
}

void CGumpScreenGame::UpdateContent()
{
    Core::Rect<int> display = Core::Platform::GetDisplayArea();
    int screenX = display.size.x - 20;
    int screenY = display.size.y - 60;

    if (g_PressedObject.LeftGump == this)
    {
        Core::Vec2<i32> offset = g_MouseManager.GetLeftDroppedOffset();

        if (g_PressedObject.LeftObject == m_Items) //resizer
        {
            g_RenderBounds.GameWindowWidth += offset.x;
            g_RenderBounds.GameWindowHeight += offset.y;

            if (g_RenderBounds.GameWindowWidth < 640)
            {
                g_RenderBounds.GameWindowWidth = 640;
            }

            if (g_RenderBounds.GameWindowWidth >= screenX)
            {
                g_RenderBounds.GameWindowWidth = screenX;
            }

            if (g_RenderBounds.GameWindowHeight < 480)
            {
                g_RenderBounds.GameWindowHeight = 480;
            }

            if (g_RenderBounds.GameWindowHeight >= screenY)
            {
                g_RenderBounds.GameWindowHeight = screenY;
            }

            CGumpOptions* opt = (CGumpOptions*)g_GumpManager.UpdateGump(0, 0, GT_OPTIONS);

            if (opt != nullptr)
            {
                opt->m_GameWindowWidth->m_Entry.SetTextW(
                    std::to_wstring(g_RenderBounds.GameWindowWidth));
                opt->m_GameWindowHeight->m_Entry.SetTextW(
                    std::to_wstring(g_RenderBounds.GameWindowHeight));
            }
        }
        else //scope
        {
            g_RenderBounds.GameWindowPosX += offset.x;
            g_RenderBounds.GameWindowPosY += offset.y;

            if (g_RenderBounds.GameWindowPosX < 0)
            {
                g_RenderBounds.GameWindowPosX = 0;
            }

            if (g_RenderBounds.GameWindowPosY < 0)
            {
                g_RenderBounds.GameWindowPosY = 0;
            }

            if (g_RenderBounds.GameWindowPosX + g_RenderBounds.GameWindowWidth >
                g_gameWindow.GetSize().x)
            {
                g_RenderBounds.GameWindowPosX =
                    g_gameWindow.GetSize().x - g_RenderBounds.GameWindowWidth;
            }

            if (g_RenderBounds.GameWindowPosY + g_RenderBounds.GameWindowHeight >
                g_gameWindow.GetSize().y)
            {
                g_RenderBounds.GameWindowPosY =
                    g_gameWindow.GetSize().y - g_RenderBounds.GameWindowHeight;
            }
        }
    }
}

void CGumpScreenGame::InitToolTip()
{
    if (!g_ConfigManager.UseToolTips || g_SelectedObject.Object == nullptr)
    {
        return;
    }

    if (g_SelectedObject.Serial == ID_GS_RESIZE)
    {
        g_ToolTip.Set(L"Resize game window", 100);
    }
}

void CGumpScreenGame::Draw()
{
    //Рамка игрового окна
    g_Orion.DrawGump(
        0x0A8D,
        0,
        g_RenderBounds.GameWindowPosX - 4,
        g_RenderBounds.GameWindowPosY - 4,
        0,
        g_RenderBounds.GameWindowHeight + 8);
    g_Orion.DrawGump(
        0x0A8D,
        0,
        g_RenderBounds.GameWindowPosX + g_RenderBounds.GameWindowWidth,
        g_RenderBounds.GameWindowPosY - 4,
        0,
        g_RenderBounds.GameWindowHeight + 8);

    g_Orion.DrawGump(
        0x0A8C,
        0,
        g_RenderBounds.GameWindowPosX - 4,
        g_RenderBounds.GameWindowPosY - 4,
        g_RenderBounds.GameWindowWidth + 4,
        0);
    g_Orion.DrawGump(
        0x0A8C,
        0,
        g_RenderBounds.GameWindowPosX - 4,
        g_RenderBounds.GameWindowPosY + g_RenderBounds.GameWindowHeight,
        g_RenderBounds.GameWindowWidth + 8,
        0);

    u16 resizeGumpID = 0x0837; //button
    if (g_ConfigManager.LockResizingGameWindow)
    {
        resizeGumpID = 0x082C; //lock
    }
    else if (g_SelectedObject.Object == m_Items)
    {
        resizeGumpID++; //lighted button
    }

    g_Orion.DrawGump(
        resizeGumpID,
        0,
        g_RenderBounds.GameWindowPosX + g_RenderBounds.GameWindowWidth - 3,
        g_RenderBounds.GameWindowPosY + g_RenderBounds.GameWindowHeight - 3);
}

CRenderObject* CGumpScreenGame::Select()
{
    CRenderObject* selected = nullptr;

    if (!g_ConfigManager.LockResizingGameWindow)
    {
        if (g_Orion.GumpPixelsInXY(
                0x0837,
                g_RenderBounds.GameWindowPosX + g_RenderBounds.GameWindowWidth - 3,
                g_RenderBounds.GameWindowPosY + g_RenderBounds.GameWindowHeight - 3))
        {
            selected = (CRenderObject*)m_Items;
        }
        else if (g_Orion.GumpPixelsInXY(
                     0x0A8D,
                     g_RenderBounds.GameWindowPosX - 4,
                     g_RenderBounds.GameWindowPosY - 4,
                     0,
                     g_RenderBounds.GameWindowHeight + 8))
        {
            selected = (CRenderObject*)m_Items->m_Next;
        }
        else if (g_Orion.GumpPixelsInXY(
                     0x0A8D,
                     g_RenderBounds.GameWindowPosX + g_RenderBounds.GameWindowWidth,
                     g_RenderBounds.GameWindowPosY - 4,
                     0,
                     g_RenderBounds.GameWindowHeight + 8))
        {
            selected = (CRenderObject*)m_Items->m_Next;
        }
        else if (g_Orion.GumpPixelsInXY(
                     0x0A8C,
                     g_RenderBounds.GameWindowPosX - 4,
                     g_RenderBounds.GameWindowPosY - 4,
                     g_RenderBounds.GameWindowWidth + 8,
                     0))
        {
            selected = (CRenderObject*)m_Items->m_Next;
        }
        else if (g_Orion.GumpPixelsInXY(
                     0x0A8C,
                     g_RenderBounds.GameWindowPosX - 4,
                     g_RenderBounds.GameWindowPosY + g_RenderBounds.GameWindowHeight,
                     g_RenderBounds.GameWindowWidth + 8,
                     0))
        {
            selected = (CRenderObject*)m_Items->m_Next;
        }

        if (selected != nullptr)
        {
            g_SelectedObject.Init(selected, this);
        }
    }

    return selected;
}

void CGumpScreenGame::OnLeftMouseButtonDown()
{
    //CGump::OnLeftMouseButtonDown();

    if (g_GumpConsoleType != nullptr)
    {
        g_GumpManager.MoveToBack(g_GumpConsoleType);
    }
}

void CGumpScreenGame::OnLeftMouseButtonUp()
{
    Core::Rect<int> display = Core::Platform::GetDisplayArea();
    int screenX = display.size.x - 20;
    int screenY = display.size.y - 60;

    Core::Vec2<i32> offset = g_MouseManager.GetLeftDroppedOffset();

    if (g_PressedObject.LeftObject == m_Items) //resizer
    {
        g_ConfigManager.GameWindowWidth  = g_ConfigManager.GameWindowWidth + offset.x;
        g_ConfigManager.GameWindowHeight = g_ConfigManager.GameWindowHeight + offset.y;

        if (g_ConfigManager.GameWindowWidth < 640)
        {
            g_ConfigManager.GameWindowWidth = 640;
        }

        if (g_ConfigManager.GameWindowWidth >= screenX)
        {
            g_ConfigManager.GameWindowWidth = screenX;
        }

        if (g_ConfigManager.GameWindowHeight < 480)
        {
            g_ConfigManager.GameWindowHeight = 480;
        }

        if (g_ConfigManager.GameWindowHeight >= screenY)
        {
            g_ConfigManager.GameWindowHeight = screenY;
        }

        if (GameVars::GetClientVersion() >= CV_200)
        {
            CPacketGameWindowSize().Send();
        }
    }
    else //scope
    {
        g_ConfigManager.GameWindowX = g_ConfigManager.GameWindowX + offset.x;
        g_ConfigManager.GameWindowY = g_ConfigManager.GameWindowY + offset.y;

        if (g_ConfigManager.GameWindowX < 1)
        {
            g_ConfigManager.GameWindowX = 0;
        }

        if (g_ConfigManager.GameWindowY < 1)
        {
            g_ConfigManager.GameWindowY = 0;
        }

        if (g_ConfigManager.GameWindowX + g_ConfigManager.GameWindowWidth >
            g_gameWindow.GetSize().x)
        {
            g_ConfigManager.GameWindowX =
                g_gameWindow.GetSize().x - g_ConfigManager.GameWindowWidth;
        }

        if (g_ConfigManager.GameWindowY + g_ConfigManager.GameWindowHeight >
            g_gameWindow.GetSize().y)
        {
            g_ConfigManager.GameWindowY =
                g_gameWindow.GetSize().y - g_ConfigManager.GameWindowHeight;
        }
    }
}
