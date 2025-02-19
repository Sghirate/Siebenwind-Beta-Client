#pragma once

#include "Core/Input.h"
#include "Platform.h"
#include "GUI/GUI.h"
#include "RenderObject.h"

class CBaseGUI;

#define GUMP_BUTTON_EVENT_C OnButton(int serial)
#define GUMP_CHECKBOX_EVENT_C OnCheckbox(int serial, bool state)
#define GUMP_RADIO_EVENT_C OnRadio(int serial, bool state)
#define GUMP_TEXT_ENTRY_EVENT_C OnTextEntry(int serial)
#define GUMP_SLIDER_CLICK_EVENT_C OnSliderClick(int serial)
#define GUMP_SLIDER_MOVE_EVENT_C OnSliderMove(int serial)
#define GUMP_RESIZE_START_EVENT_C OnResizeStart(int serial)
#define GUMP_RESIZE_EVENT_C OnResize(int serial)
#define GUMP_RESIZE_END_EVENT_C OnResizeEnd(int serial)
#define GUMP_DIRECT_HTML_LINK_EVENT_C OnDirectHTMLLink(u16 link)
#define GUMP_COMBOBOX_SELECTION_EVENT_C OnComboboxSelection(int serial)
#define GUMP_SCROLL_BUTTON_EVENT_C OnScrollButton()

#define GUMP_BUTTON_EVENT_H virtual void GUMP_BUTTON_EVENT_C
#define GUMP_CHECKBOX_EVENT_H virtual void GUMP_CHECKBOX_EVENT_C
#define GUMP_RADIO_EVENT_H virtual void GUMP_RADIO_EVENT_C
#define GUMP_TEXT_ENTRY_EVENT_H virtual void GUMP_TEXT_ENTRY_EVENT_C
#define GUMP_SLIDER_CLICK_EVENT_H virtual void GUMP_SLIDER_CLICK_EVENT_C
#define GUMP_SLIDER_MOVE_EVENT_H virtual void GUMP_SLIDER_MOVE_EVENT_C
#define GUMP_RESIZE_START_EVENT_H virtual void GUMP_RESIZE_START_EVENT_C
#define GUMP_RESIZE_EVENT_H virtual void GUMP_RESIZE_EVENT_C
#define GUMP_RESIZE_END_EVENT_H virtual void GUMP_RESIZE_END_EVENT_C
#define GUMP_DIRECT_HTML_LINK_EVENT_H virtual void GUMP_DIRECT_HTML_LINK_EVENT_C
#define GUMP_COMBOBOX_SELECTION_EVENT_H virtual void GUMP_COMBOBOX_SELECTION_EVENT_C
#define GUMP_SCROLL_BUTTON_EVENT_H virtual void GUMP_SCROLL_BUTTON_EVENT_C

class CGump : public CRenderObject
{
public:
    GUMP_TYPE GumpType       = GT_NONE;
    u32 ID                   = 0;
    int MinimizedX           = 0;
    int MinimizedY           = 0;
    bool NoMove              = false;
    bool NoClose             = false;
    bool Minimized           = false;
    bool FrameCreated        = false;
    bool WantRedraw          = false;
    bool WantUpdateContent   = true;
    bool Blocked             = false;
    bool LockMoving          = false;
    int Page                 = 0;
    int Draw2Page            = 0;
    bool Transparent         = false;
    bool RemoveMark          = false;
    bool NoProcess           = false;
    bool Visible             = true;
    Core::Rect<i32> GumpRect = Core::Rect<i32>();

protected:
    CGUIButton m_Locker{ CGUIButton(0, 0, 0, 0, 0, 0) };

    CGLFrameBuffer m_FrameBuffer{ CGLFrameBuffer() };

    virtual void CalculateGumpState();

    virtual void RecalculateSize();

public:
    CGump();
    CGump(GUMP_TYPE type, u32 serial, int x, int y);
    virtual ~CGump();

    virtual void PasteClipboardData(std::wstring& data);
    static void ProcessListing();

    static bool
    ApplyTransparent(CBaseGUI* item, int page, int currentPage, const int draw2Page = 0);

    static void DrawItems(CBaseGUI* start, int currentPage, int draw2Page = 0);
    static class CRenderObject* SelectItems(CBaseGUI* start, int currentPage, int draw2Page = 0);

    static void GetItemsSize(
        CGump* gump,
        CBaseGUI* start,
        Core::Vec2<i32>& minPosition,
        Core::Vec2<i32>& maxPosition,
        Core::Vec2<i32>& offset,
        int count,
        int currentPage,
        int draw2Page);
    static void TestItemsLeftMouseDown(
        CGump* gump, CBaseGUI* start, int currentPage, int draw2Page = 0, int count = -1);
    static void
    TestItemsLeftMouseUp(CGump* gump, CBaseGUI* start, int currentPage, int draw2Page = 0);
    static void TestItemsDragging(
        CGump* gump, CBaseGUI* start, int currentPage, int draw2Page = 0, int count = -1);
    static void
    TestItemsScrolling(CGump* gump, CBaseGUI* start, bool up, int currentPage, int draw2Page = 0);

    virtual void DelayedClick(class CRenderObject* obj) {}
    virtual void PrepareContent() {}
    virtual void UpdateContent() {}
    virtual class CTextRenderer* GetTextRenderer() { return nullptr; }
    virtual void PrepareTextures();
    virtual void GenerateFrame(bool stop);
    virtual bool CanBeDisplayed() { return true; }

    void FixCoordinates();
    bool CanBeMoved();
    void DrawLocker();
    bool SelectLocker();
    bool TestLockerClick();

    virtual bool EntryPointerHere();
    virtual void Draw();
    virtual class CRenderObject* Select();
    virtual void InitToolTip() {}

    GUMP_BUTTON_EVENT_H {}
    GUMP_CHECKBOX_EVENT_H {}
    GUMP_RADIO_EVENT_H {}
    GUMP_TEXT_ENTRY_EVENT_H {}
    GUMP_SLIDER_CLICK_EVENT_H {}
    GUMP_SLIDER_MOVE_EVENT_H {}
    GUMP_RESIZE_START_EVENT_H {}
    GUMP_RESIZE_EVENT_H {}
    GUMP_RESIZE_END_EVENT_H {}
    GUMP_DIRECT_HTML_LINK_EVENT_H;
    GUMP_COMBOBOX_SELECTION_EVENT_H {}
    GUMP_SCROLL_BUTTON_EVENT_H {}

    virtual void OnLeftMouseButtonDown();
    virtual void OnLeftMouseButtonUp();
    virtual bool OnLeftMouseButtonDoubleClick() { return false; }
    virtual void OnRightMouseButtonDown() {}
    virtual void OnRightMouseButtonUp() {}
    virtual bool OnRightMouseButtonDoubleClick() { return false; }
    virtual void OnMidMouseButtonDown() {}
    virtual void OnMidMouseButtonUp() {}
    virtual bool OnMidMouseButtonDoubleClick() { return false; }
    virtual void OnMidMouseButtonScroll(bool up);
    virtual void OnDragging();
    virtual void OnTextInput(const Core::TextEvent& ev) {}
    virtual void OnKeyDown(const Core::KeyEvent& ev) {}
    virtual void OnKeyUp(const Core::KeyEvent& ev) {}
};

extern CGump* g_ResizedGump;
extern CGump* g_CurrentCheckGump;
