#include "MainScreen.h"
#include "Globals.h"
#include "BaseScreen.h"
#include "GameVars.h"
#include "GameWindow.h"
#include "Platform.h"
#include "SiebenwindClient.h"
#include "Config.h"
#include "Definitions.h"
#include "OrionUO.h"
#include "QuestArrow.h"
#include "Managers/FontsManager.h"
#include "Managers/ScreenEffectManager.h"
#include "Managers/AnimationManager.h"
#include "GUI/GUITextEntry.h"
#include "TextEngine/EntryText.h"

CMainScreen g_MainScreen;

CMainScreen::CMainScreen()
    : CBaseScreen(m_MainGump)
    , m_Account(nullptr)
    , m_Password(nullptr)
    , m_SavePassword(nullptr)
    , m_AutoLogin(nullptr)
{
    m_Password = new CEntryText(32, 0, 300);
}

CMainScreen::~CMainScreen()
{
    delete m_Password;
}

void CMainScreen::Init()
{
    g_ConfigLoaded = false;
    g_GlobalScale = 1.0;

    Load();

    Reset();
    g_gameWindow.SetTitle(SiebenwindClient::GetWindowTitle().c_str());
    g_GL.UpdateRect();

    g_EntryPointer = m_MainGump.m_PasswordFake;

    g_AnimationManager.ClearUnusedTextures(g_Ticks + 100000);

    g_QuestArrow.Enabled = false;

    g_TotalSendSize = 0;
    g_TotalRecvSize = 0;

    g_LightLevel = 0;
    g_PersonalLightLevel = 0;

    g_ScreenEffectManager.UseSunrise();
    SmoothScreenAction = 0;

    m_Gump.PrepareTextures();
}

void CMainScreen::ProcessSmoothAction(u8 action)
{
    if (action == 0xFF)
    {
        action = SmoothScreenAction;
    }

    if (action == ID_SMOOTH_MS_CONNECT)
    {
        g_Orion.Connect();
    }
    else if (action == ID_SMOOTH_MS_QUIT)
    {
        g_gameWindow.Close();
    }
}

void CMainScreen::SetAccounting(const std::string &account, const std::string &password)
{
    m_Account->SetTextA(account);
    m_Password->SetTextA(password);

    const auto len = (int)password.length();
    m_MainGump.m_PasswordFake->Clear();

    for (int i = 0; i < len; i++)
    {
        m_MainGump.m_PasswordFake->Insert(L'*');
    }
}

void CMainScreen::Paste()
{
    if (g_EntryPointer == m_MainGump.m_PasswordFake)
    {
        m_Password->Paste();

        const auto len = (int)m_Password->Length();
        g_EntryPointer->Clear();

        for (int i = 0; i < len; i++)
        {
            g_EntryPointer->Insert(L'*');
        }
    }
    else
    {
        g_EntryPointer->Paste();
    }
}

void CMainScreen::OnTextInput(const Core::TextEvent &ev)
{

    const auto ch = ev.text[0];
    if (ch >= 0x0100 || !g_FontManager.IsPrintASCII((u8)ch))
    {
        return;
    }
    if (g_EntryPointer == nullptr)
    {
        g_EntryPointer = m_MainGump.m_PasswordFake;
    }

    if (g_EntryPointer->Length() < 16) //add char to text field
    {
        if (g_EntryPointer == m_MainGump.m_PasswordFake)
        {
            if (g_EntryPointer->Insert(L'*'))
            {
                m_Password->Insert(ch);
            }
        }
        else
        {
            g_EntryPointer->Insert(ch);
        }
    }

    m_Gump.WantRedraw = true;
}

void CMainScreen::OnKeyDown(const Core::KeyEvent &ev)
{

    if (g_EntryPointer == nullptr)
    {
        g_EntryPointer = m_MainGump.m_PasswordFake;
    }
    switch (ev.key)
    {
        case Core::EKey::Key_Tab:
        {
            if (g_EntryPointer == m_Account)
            {
                g_EntryPointer = m_MainGump.m_PasswordFake;
            }
            else
            {
                g_EntryPointer = m_Account;
            }
            break;
        }
        case Core::EKey::Key_Return:
        case Core::EKey::Key_Return2:
        {
            CreateSmoothAction(ID_SMOOTH_MS_CONNECT);
            break;
        }
        default:
        {
            if (g_EntryPointer == m_MainGump.m_PasswordFake)
            {
                m_Password->OnKey(nullptr, ev.key);
            }

            g_EntryPointer->OnKey(nullptr, ev.key);
            break;
        }
    }
    m_Gump.WantRedraw = true;
}

void CMainScreen::Load()
{
    m_AutoLogin->Checked = uo_auto_login.GetValue() > 0;

    m_Account->SetTextA(uo_login.GetValue());
    m_Account->SetPos(checked_cast<int>(uo_login.GetValue().length()));

    m_MainGump.m_PasswordFake->SetTextA("");
    m_MainGump.m_PasswordFake->SetPos(0);

    const size_t len = uo_password.GetValue().length();
    if (len != 0)
    {
        m_Password->SetTextA(uo_password.GetValue());
        for (int zv = 0; zv < len; zv++)
        {
            m_MainGump.m_PasswordFake->Insert(L'*');
        }
        m_Password->SetPos(checked_cast<int>(len));
    }
    else
    {
        m_Password->SetTextA("");
        m_Password->SetPos(0);
    }

    m_SavePassword->Checked = uo_save_password.GetValue() > 0;
    if (!m_SavePassword->Checked)
    {
        m_Password->SetTextW({});
        m_MainGump.m_PasswordFake->SetTextW({});
    }
}

void CMainScreen::Save()
{
    if (m_AutoLogin) 
    {
        uo_auto_login.SetValue(m_AutoLogin->Checked ? 1 : 0);
    }
    if (m_SavePassword)
    {
        uo_save_password.SetValue(m_SavePassword->Checked ? 1 : 0);
    }
    if (m_Password)
    {
        uo_password.SetValue(uo_save_password.GetValue() > 0 ? m_Password->GetTextA() : "");
    }
    if (m_Account)
    {
        uo_login.SetValue(m_Account->GetTextA());
    }
}

void CMainScreen::Reset() const
{
    g_gameWindow.Restore();
    g_gameWindow.SetSize(Core::Vec2<i32>(640, 480));
    g_gameWindow.SetIsResizeable(false);
}
