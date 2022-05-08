#include "SelectTownScreen.h"
#include "CreateCharacterScreen.h"
#include "GameVars.h"
#include "GameWindow.h"
#include "Config.h"
#include "OrionUO.h"
#include "CityList.h"
#include "Managers/ScreenEffectManager.h"
#include "Network/Packets.h"

CSelectTownScreen g_SelectTownScreen;

CSelectTownScreen::CSelectTownScreen()
    : CBaseScreen(m_SelectTownGump)
{
}

CSelectTownScreen::~CSelectTownScreen()
{
}

void CSelectTownScreen::Init()
{
    if (GameVars::GetClientVersion() >= CV_70130)
    {
        m_City = g_CityList.GetCity(0);
    }
    else
    {
        m_City = g_CityList.GetCity(3);

        if (m_City == nullptr)
        {
            m_City = g_CityList.GetCity(0);
        }
    }

    g_ScreenEffectManager.UseSunrise();
    SmoothScreenAction = 0;

    m_Gump.PrepareTextures();
    m_Gump.WantUpdateContent = true;
}

void CSelectTownScreen::ProcessSmoothAction(u8 action)
{
    if (action == 0xFF)
    {
        action = SmoothScreenAction;
    }

    if (action == ID_SMOOTH_STS_QUIT)
    {
        g_gameWindow.Destroy();
    }
    else if (action == ID_SMOOTH_STS_GO_SCREEN_CHARACTER)
    {
        g_Orion.InitScreen(GS_CHARACTER);
    }
    else if (action == ID_SMOOTH_STS_GO_SCREEN_GAME_CONNECT)
    {
        CPacketCreateCharacter(g_CreateCharacterScreen.Name).Send();
        g_Orion.InitScreen(GS_GAME_CONNECT);
    }
}
