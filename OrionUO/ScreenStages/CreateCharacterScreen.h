#pragma once

#include "Platform.h"
#include "BaseScreen.h"
#include "Gumps/GumpScreenCreateCharacter.h"
#include <string>

class CCreateCharacterScreen : public CBaseScreen
{
protected:
    int m_StyleSelection = 0;

public:
    int GetStyleSelection() { return m_StyleSelection; };
    void SetStyleSelection(int val);

protected:
    int m_ColorSelection = 0;

public:
    int GetColorSelection() { return m_ColorSelection; };
    void SetColorSelection(int val);
    std::string Name = "";

private:
    CGumpScreenCreateCharacter m_CreateCharacterGump;

public:
    CCreateCharacterScreen();
    virtual ~CCreateCharacterScreen();

    static const u8 ID_SMOOTH_CCS_QUIT = 1;
    static const u8 ID_SMOOTH_CCS_GO_SCREEN_CHARACTER = 2;
    static const u8 ID_SMOOTH_CCS_GO_SCREEN_CONNECT = 3;
    static const u8 ID_SMOOTH_CCS_GO_SCREEN_SELECT_TOWN = 4;

    void Init();
    void ProcessSmoothAction(u8 action = 0xFF);
    virtual void OnLeftMouseButtonDown();
     void OnTextInput(const Core::TextEvent &ev)  override;
     void OnKeyDown(const Core::KeyEvent &ev)  override;
};

extern CCreateCharacterScreen g_CreateCharacterScreen;
