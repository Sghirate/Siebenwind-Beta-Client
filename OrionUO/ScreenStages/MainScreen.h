#pragma once

#include "BaseScreen.h"
#include "Platform.h"
#include "Gumps/GumpScreenMain.h"

class CEntryText;
class CGUICheckbox;

class CMainScreen : public CBaseScreen
{
private:
    CGumpScreenMain m_MainGump;

    void Load();

public:
    enum
    {
        ID_SMOOTH_MS_QUIT = 1,
        ID_SMOOTH_MS_CONNECT = 2,
    };

    CEntryText *m_Account = nullptr;
    CEntryText *m_Password = nullptr;
    CGUICheckbox *m_SavePassword = nullptr;
    CGUICheckbox *m_AutoLogin = nullptr;

public:
    CMainScreen();
    virtual ~CMainScreen();

    void SetAccounting(const std::string &account, const std::string &password);
    void Paste();
    void ProcessSmoothAction(u8 action = 0xFF);
    void Init();

     void OnTextInput(const Core::TextEvent &ev)  override;
     void OnKeyDown(const Core::KeyEvent &ev)  override;

    void Save();
    void Reset() const;
};

extern CMainScreen g_MainScreen;
