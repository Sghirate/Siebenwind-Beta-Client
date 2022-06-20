#include "CharacterListScreen.h"
#include "ConnectionScreen.h"
#include "GameWindow.h"
#include "MainScreen.h"
#include "OrionUO.h"
#include "ServerList.h"
#include "CharacterList.h"
#include "Managers/ScreenEffectManager.h"
#include "SiebenwindClient.h"

CCharacterListScreen g_CharacterListScreen;

CCharacterListScreen::CCharacterListScreen()
    : CBaseScreen(m_CharacterListGump)
{
}

CCharacterListScreen::~CCharacterListScreen()
{
}

void CCharacterListScreen::Init()
{
    std::string title =
        SiebenwindClient::GetWindowTitle() + " - " + g_MainScreen.m_Account->c_str();

    CServer* server = g_ServerList.GetSelectedServer();

    if (server != nullptr)
    {
        title += "(" + server->Name + ")";
    }

    g_gameWindow.SetTitle(title.c_str());

    g_CharacterList.Selected = 0;

    g_ScreenEffectManager.UseSunrise();
    SmoothScreenAction = 0;

    m_Gump.PrepareTextures();
    m_Gump.WantUpdateContent = true;
}

void CCharacterListScreen::ProcessSmoothAction(u8 action)
{
    if (action == 0xFF)
    {
        action = SmoothScreenAction;
    }

    if (action == ID_SMOOTH_CLS_QUIT)
    {
        g_gameWindow.Close();
    }
    else if (action == ID_SMOOTH_CLS_CONNECT)
    {
        g_Orion.Connect();
    }
    else if (action == ID_SMOOTH_CLS_SELECT_CHARACTER)
    {
        if (g_CharacterList.GetName(g_CharacterList.Selected).length() == 0u)
        {
            g_Orion.InitScreen(GS_PROFESSION_SELECT);
        }
        else
        {
            g_Orion.CharacterSelection(g_CharacterList.Selected);
        }
    }
    else if (action == ID_SMOOTH_CLS_GO_SCREEN_PROFESSION_SELECT)
    {
        g_Orion.InitScreen(GS_PROFESSION_SELECT);
    }
    else if (action == ID_SMOOTH_CLS_GO_SCREEN_DELETE)
    {
        if (g_CharacterList.GetSelectedName().length() != 0u)
        {
            g_Orion.InitScreen(GS_DELETE);
            g_ConnectionScreen.SetType(CST_CHARACTER_LIST);
        }
    }
}

void CCharacterListScreen::OnKeyDown(const Core::KeyEvent& ev)
{
    m_Gump.OnKeyDown(ev);
    if (ev.key == Core::EKey::Key_Return || ev.key == Core::EKey::Key_Return2)
    {
        CreateSmoothAction(ID_SMOOTH_CLS_SELECT_CHARACTER);
    }
}
