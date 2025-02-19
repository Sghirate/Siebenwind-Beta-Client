#pragma once

#include "Platform.h"
#include "BaseScreen.h"
#include "Gumps/GumpScreenConnection.h"
#include <string>

class CConnectionScreen : public CBaseScreen
{
protected:
    bool m_ConnectionFailed = false;

public:
    bool GetConnectionFailed() { return m_ConnectionFailed; };
    void SetConnectionFailed(bool val);

protected:
    bool m_Connected = false;

public:
    bool GetConnected() { return m_Connected; };
    void SetConnected(bool val);

protected:
    bool m_Completed = false;

public:
    bool GetCompleted() { return m_Completed; };
    void SetCompleted(bool val);

protected:
    int m_ErrorCode = 0;

public:
    int GetErrorCode() { return m_ErrorCode; };
    void SetErrorCode(int val);

protected:
    CONNECTION_SCREEN_TYPE m_Type = CST_LOGIN;

public:
    CONNECTION_SCREEN_TYPE GetType() { return m_Type; };
    void SetType(CONNECTION_SCREEN_TYPE val);

protected:
    std::string m_Text = "";

public:
    std::string GetTextA() { return m_Text; };
    void SetTextA(const std::string &val);

private:
    CGumpScreenConnection m_ConnectionGump;

public:
    CConnectionScreen();
    ~CConnectionScreen();

    static const u8 ID_SMOOTH_CS_GO_SCREEN_MAIN = 1;
    static const u8 ID_SMOOTH_CS_GO_SCREEN_CHARACTER = 2;
    static const u8 ID_SMOOTH_CS_GO_SCREEN_PROFESSION = 3;
    static const u8 ID_SMOOTH_CS_SEND_DELETE = 4;

    void Init();
    void ProcessSmoothAction(u8 action = 0xFF);

    virtual void OnKeyDown(const Core::KeyEvent &ev);
};

extern CConnectionScreen g_ConnectionScreen;
