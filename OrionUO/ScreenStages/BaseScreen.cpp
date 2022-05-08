#include "BaseScreen.h"
#include "Profiler.h"
#include "SelectedObject.h"
#include "Managers/MouseManager.h"
#include "Managers/ScreenEffectManager.h"

CBaseScreen *g_CurrentScreen = nullptr;

CBaseScreen::CBaseScreen(CGump &gump)
    : m_Gump(gump)
{
}

void CBaseScreen::Render()
{
    PROFILER_EVENT();

    g_GL.BeginDraw();
    if (DrawSmoothMonitor() != 0)
    {
        return;
    }

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    m_Gump.Draw();
    InitToolTip();
    DrawSmoothMonitorEffect();
    g_MouseManager.Draw(CursorGraphic);
    g_GL.EndDraw();
}

void CBaseScreen::SelectObject()
{

    g_SelectedObject.Clear();
    CRenderObject *selected = m_Gump.Select();
    if (selected != nullptr)
    {
        g_SelectedObject.Init(selected, &m_Gump);
    }

    if (g_SelectedObject.Object != g_LastSelectedObject.Object)
    {
        if (g_SelectedObject.Object != nullptr)
        {
            g_SelectedObject.Object->OnMouseEnter();
        }

        if (g_LastSelectedObject.Object != nullptr)
        {
            g_LastSelectedObject.Object->OnMouseExit();
        }
    }

    g_LastSelectedObject.Init(g_SelectedObject);
}

int CBaseScreen::DrawSmoothMonitor()
{
    if ((g_ScreenEffectManager.Process() != 0) && (SmoothScreenAction != 0u))
    {
        ProcessSmoothAction();
        g_GL.EndDraw();
        return 1;
    }
    return 0;
}

void CBaseScreen::DrawSmoothMonitorEffect()
{
    g_ScreenEffectManager.Draw();
}

void CBaseScreen::CreateSmoothAction(u8 action)
{
    if (g_ScreenEffectManager.UseSunset())
    {
        SmoothScreenAction = action;
    }
    else
    {
        ProcessSmoothAction(action);
    }
}
