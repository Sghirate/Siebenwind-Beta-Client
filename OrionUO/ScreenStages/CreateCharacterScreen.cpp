#include "CreateCharacterScreen.h"
#include "ConnectionScreen.h"
#include "GameWindow.h"
#include "OrionUO.h"
#include "SelectedObject.h"
#include "Managers/FontsManager.h"
#include "Managers/ScreenEffectManager.h"
#include "Managers/CreateCharacterManager.h"

CCreateCharacterScreen g_CreateCharacterScreen;

CCreateCharacterScreen::CCreateCharacterScreen()
    : CBaseScreen(m_CreateCharacterGump)
{
}

CCreateCharacterScreen::~CCreateCharacterScreen()
{
}

void CCreateCharacterScreen::SetStyleSelection(int val)
{
    m_StyleSelection = val;
    m_Gump.WantUpdateContent = true;
}

void CCreateCharacterScreen::SetColorSelection(int val)
{
    m_ColorSelection = val;
    m_Gump.WantUpdateContent = true;
}

void CCreateCharacterScreen::Init()
{
    g_CreateCharacterManager.Clear();

    Name = "";
    m_StyleSelection = 0;
    m_ColorSelection = 0;

    g_ScreenEffectManager.UseSunrise();
    SmoothScreenAction = 0;

    m_Gump.PrepareTextures();
    m_Gump.WantUpdateContent = true;
}

void CCreateCharacterScreen::ProcessSmoothAction(u8 action)
{
    if (action == 0xFF)
    {
        action = SmoothScreenAction;
    }

    if (action == ID_SMOOTH_CCS_QUIT)
    {
        g_gameWindow.Close();
    }
    else if (action == ID_SMOOTH_CCS_GO_SCREEN_CHARACTER)
    {
        g_Orion.InitScreen(GS_CHARACTER);
    }
    else if (action == ID_SMOOTH_CCS_GO_SCREEN_CONNECT)
    {
        g_Orion.InitScreen(GS_GAME_CONNECT);
        g_ConnectionScreen.SetType(CST_GAME);
        g_ConnectionScreen.SetConnectionFailed(true);
        g_ConnectionScreen.SetErrorCode(1);
    }
    else if (action == ID_SMOOTH_CCS_GO_SCREEN_SELECT_TOWN)
    {
        g_Orion.InitScreen(GS_SELECT_TOWN);
    }
}

void CCreateCharacterScreen::OnLeftMouseButtonDown()
{
    CBaseScreen::OnLeftMouseButtonDown();

    if (g_SelectedObject.Serial == 0)
    {
        if (m_StyleSelection != 0)
        {
            m_StyleSelection = 0;
            m_Gump.WantUpdateContent = true;
        }
    }
}

void CCreateCharacterScreen::OnTextInput(const Core::TextEvent &ev)
{
    const auto ch = ev.text[0];
    if (ch >= 0x0100 || !g_FontManager.IsPrintASCII((u8)ch))
    {
        return;
    }
    if (g_EntryPointer == nullptr)
    {
        return;
    }

    if (g_EntryPointer->Length() < 20)
    { //add char to text field
        g_EntryPointer->Insert(ev.wtext[0]);
    }

    Name = g_EntryPointer->c_str();
    m_Gump.WantRedraw = true;
}

void CCreateCharacterScreen::OnKeyDown(const Core::KeyEvent &ev)
{
    if (g_EntryPointer != nullptr)
    {
        g_EntryPointer->OnKey(&m_Gump, ev.key);
        Name = g_EntryPointer->c_str();
        m_Gump.WantRedraw = true;
    }
}
