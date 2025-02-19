#include "ServerScreen.h"
#include "MainScreen.h"
#include "GameWindow.h"
#include "SiebenwindClient.h"
#include "OrionUO.h"
#include "ServerList.h"
#include "Managers/ScreenEffectManager.h"

CServerScreen g_ServerScreen;

CServerScreen::CServerScreen()
    : CBaseScreen(m_ServerGump)
{
}

CServerScreen::~CServerScreen()
{
}

void CServerScreen::Init()
{
    g_gameWindow.SetTitle((SiebenwindClient::GetWindowTitle() + " - " + g_MainScreen.m_Account->c_str()).c_str());
    g_ScreenEffectManager.UseSunrise();
    SmoothScreenAction = 0;

    m_Gump.PrepareTextures();
    m_Gump.WantUpdateContent = true;
}

void CServerScreen::OnKeyDown(const Core::KeyEvent &ev)
{

    m_Gump.OnKeyDown(ev);
    if (ev.key == Core::EKey::Key_Return || ev.key == Core::EKey::Key_Return2)
    {
        SelectionServerTempValue = g_ServerList.LastServerIndex;
        CreateSmoothAction(ID_SMOOTH_SS_SELECT_SERVER);
    }
}

void CServerScreen::ProcessSmoothAction(u8 action)
{

    if (action == 0xFF)
    {
        action = SmoothScreenAction;
    }

    if (action == ID_SMOOTH_SS_SELECT_SERVER)
    {
        g_Orion.ServerSelection(SelectionServerTempValue);
    }
    else if (action == ID_SMOOTH_SS_QUIT)
    {
        g_gameWindow.Close();
    }
    else if (action == ID_SMOOTH_SS_GO_SCREEN_MAIN)
    {
        g_Orion.Disconnect();
        g_Orion.InitScreen(GS_MAIN);
    }
}
