#pragma once

#include "../Platform.h"
#include "../BaseQueue.h"
#include "../Gumps/Gump.h"

class CBaseScreen : public CBaseQueue
{
public:
    u8 SmoothScreenAction = 0;
    u16 CursorGraphic = 0x2073;

protected:
    CGump &m_Gump;

public:
    CBaseScreen(CGump &gump);
    virtual ~CBaseScreen() {}

    virtual void PrepareContent() { m_Gump.PrepareContent(); }
    virtual void UpdateContent() { m_Gump.UpdateContent(); }
    virtual void Init() {}
    virtual void InitToolTip() { m_Gump.InitToolTip(); }
    virtual void Render();
    virtual void SelectObject();
    virtual void CreateSmoothAction(u8 action);
    virtual void ProcessSmoothAction(u8 action = 0xFF) {}
    virtual int DrawSmoothMonitor();
    virtual void DrawSmoothMonitorEffect();

    virtual void OnLeftMouseButtonDown() { m_Gump.OnLeftMouseButtonDown(); }
    virtual void OnLeftMouseButtonUp()
    {
        m_Gump.OnLeftMouseButtonUp();
        m_Gump.WantRedraw = true;
    }
    virtual bool OnLeftMouseButtonDoubleClick() { return m_Gump.OnLeftMouseButtonDoubleClick(); }
    virtual void OnRightMouseButtonDown() { m_Gump.OnRightMouseButtonDown(); }
    virtual void OnRightMouseButtonUp() { m_Gump.OnRightMouseButtonUp(); }
    virtual bool OnRightMouseButtonDoubleClick() { return m_Gump.OnRightMouseButtonDoubleClick(); }
    virtual void OnMidMouseButtonDown() { m_Gump.OnMidMouseButtonDown(); }
    virtual void OnMidMouseButtonUp() { m_Gump.OnMidMouseButtonUp(); }
    virtual bool OnMidMouseButtonDoubleClick() { return m_Gump.OnMidMouseButtonDoubleClick(); }
    virtual void OnMidMouseButtonScroll(bool up) { m_Gump.OnMidMouseButtonScroll(up); }
    virtual void OnDragging() { m_Gump.OnDragging(); }
    virtual void OnTextInput(const TextEvent &ev) { m_Gump.OnTextInput(ev); }
    virtual void OnKeyDown(const KeyEvent &ev) { m_Gump.OnKeyDown(ev); }
    virtual void OnKeyUp(const KeyEvent &ev) { m_Gump.OnKeyUp(ev); }
};

extern CBaseScreen *g_CurrentScreen;
