#include "ConnectionScreen.h"
#include "OrionUO.h"
#include "CharacterList.h"
#include "Managers/ScreenEffectManager.h"
#include "Network/Packets.h"

CConnectionScreen g_ConnectionScreen;

CConnectionScreen::CConnectionScreen()
    : CBaseScreen(m_ConnectionGump)
{
}

CConnectionScreen::~CConnectionScreen()
{
}

void CConnectionScreen::Init()
{
    m_Text = "";
    m_ConnectionFailed = false;
    m_Connected = false;
    m_Completed = false;
    m_ErrorCode = 0;
    m_Type = CST_LOGIN;

    g_ScreenEffectManager.UseSunrise();
    SmoothScreenAction = 0;

    m_Gump.PrepareTextures();
    m_Gump.WantUpdateContent = true;
}

void CConnectionScreen::SetConnectionFailed(bool val)
{
    m_ConnectionFailed = val;
    m_Gump.WantUpdateContent = true;
}

void CConnectionScreen::SetConnected(bool val)
{
    m_Connected = val;
    m_Gump.WantUpdateContent = true;
}

void CConnectionScreen::SetCompleted(bool val)
{
    m_Completed = val;
    m_Gump.WantUpdateContent = true;
}

void CConnectionScreen::SetErrorCode(int val)
{
    m_ErrorCode = val;
    m_Gump.WantUpdateContent = true;
}

void CConnectionScreen::SetType(CONNECTION_SCREEN_TYPE val)
{
    m_Type = val;
    m_Gump.WantUpdateContent = true;
}

void CConnectionScreen::SetTextA(const std::string &val)
{
    m_Text = val;
    m_Gump.WantUpdateContent = true;
}

void CConnectionScreen::ProcessSmoothAction(u8 action)
{
    if (action == 0xFF)
    {
        action = SmoothScreenAction;
    }

    if (action == ID_SMOOTH_CS_GO_SCREEN_MAIN)
    {
        g_Orion.InitScreen(GS_MAIN);
    }
    else if (action == ID_SMOOTH_CS_GO_SCREEN_CHARACTER)
    {
        g_Orion.InitScreen(GS_CHARACTER);
    }
    else if (action == ID_SMOOTH_CS_GO_SCREEN_PROFESSION)
    {
        g_Orion.InitScreen(GS_PROFESSION_SELECT);
    }
    else if (action == ID_SMOOTH_CS_SEND_DELETE)
    {
        CPacketDeleteCharacter(g_CharacterList.Selected).Send();
    }
}

void CConnectionScreen::OnKeyDown(const Core::KeyEvent &ev)
{
    switch (ev.key)
    {
        case Core::EKey::Key_Return:
        case Core::EKey::Key_Return2:
        {
            if (m_Type == CST_CHARACTER_LIST)
            {
                CreateSmoothAction(ID_SMOOTH_CS_SEND_DELETE);
            }
            else if (m_Type == CST_SELECT_PROFESSOIN)
            {
                CreateSmoothAction(ID_SMOOTH_CS_GO_SCREEN_PROFESSION);
            }
            else if (m_Type == CST_GAME)
            {
                if (m_ErrorCode > 0)
                {
                    CreateSmoothAction(ID_SMOOTH_CS_GO_SCREEN_CHARACTER);
                }
            }
            else if (m_Type == CST_CONLOST || m_ConnectionFailed)
            {
                CreateSmoothAction(ID_SMOOTH_CS_GO_SCREEN_MAIN);
            }

            break;
        }
        case Core::EKey::Key_Escape:
        {
            if (m_Type == CST_CHARACTER_LIST)
            {
                CreateSmoothAction(ID_SMOOTH_CS_GO_SCREEN_CHARACTER);
            }
            else if (m_Type == CST_SELECT_PROFESSOIN)
            {
                CreateSmoothAction(ID_SMOOTH_CS_GO_SCREEN_PROFESSION);
            }
            else if (m_Type == CST_GAME)
            {
                if (m_ErrorCode > 0)
                {
                    CreateSmoothAction(ID_SMOOTH_CS_GO_SCREEN_CHARACTER);
                }
            }
            else if (m_Type == CST_CONLOST || m_ConnectionFailed)
            {
                CreateSmoothAction(ID_SMOOTH_CS_GO_SCREEN_MAIN);
            }

            break;
        }
        default:
            break;
    }
}
