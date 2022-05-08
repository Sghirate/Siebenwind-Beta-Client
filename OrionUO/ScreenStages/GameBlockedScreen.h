#pragma once

#include "BaseScreen.h"
#include "Platform.h"

class CGameBlockedScreen : public CBaseScreen
{
public:
    u8 Code = 0;
    CGump m_GameBlockedScreenGump;

    CGameBlockedScreen();
    ~CGameBlockedScreen();

    virtual void Init();
     void Render()  override;
     void SelectObject()  override;
    virtual void OnLeftMouseButtonDown();
    virtual void OnLeftMouseButtonUp();
     void OnTextInput(const Core::TextEvent &ev)  override;
     void OnKeyDown(const Core::KeyEvent &ev)  override;
};

extern CGameBlockedScreen g_GameBlockedScreen;
