#ifndef WISP_WIN_H
#define WISP_WIN_H

#include "Core/Minimal.h"
#include <SDL_video.h>
#include "Platform.h"
#include "plugin/plugininterface.h"

#define PUSH_EVENT(id, data1, data2) Wisp::CWindow::PushEvent(id, (void *)(data1), (void *)(data2))
#define PLUGIN_EVENT(id, data) Wisp::CWindow::PluginEvent(id, (const void *)(data))

void GetDisplaySize(int *x, int *y);

namespace Wisp
{
class CWindow
{
public:
    bool NoResize = false;

    // FIXME: Last API specific public surface on windowing stuff
    static WindowHandle Handle;
    SDL_Window *m_window = nullptr;
    bool OnWindowProc(SDL_Event &ev);

    Core::Vec2<i32> GetSize() { return m_Size; };
    void SetSize(const Core::Vec2<i32> &size);
    Core::Vec2<i32> GetMinSize() { return m_MinSize; };
    void SetMinSize(const Core::Vec2<i32> &newMinSize);
    Core::Vec2<i32> GetMaxSize() { return m_MaxSize; };
    void SetMaxSize(const Core::Vec2<i32> &newMaxSize);
    void GetPositionSize(int *x, int *y, int *width, int *height);
    void SetPositionSize(int x, int y, int width, int height);
    void MaximizeWindow();

public:
    CWindow();
    virtual ~CWindow();

    bool Create(
        const char *className,
        const char *title,
        bool showCursor = false,
        int width = 800,
        int height = 600);
    void Destroy();

    static u32 PushEvent(u32 id, void *data1 = nullptr, void *data2 = nullptr);
    static u32 PluginEvent(u32 id, const void *data = nullptr);

    void ShowMessage(const std::string &text, const std::string &title);
    void ShowMessage(const std::wstring &text, const std::wstring &title);
    void SetMinSize(int width, int height);
    void SetMaxSize(int width, int height);
    bool IsActive() const;
    void SetTitle(const std::string &text) const;
    void ShowWindow(bool show) const;
    bool IsMinimizedWindow() const;
    bool IsMaximizedWindow() const;
    void CreateTimer(u32 id, int delay);
    void RemoveTimer(u32 id);
    void Raise();

protected:
    Core::Vec2<i32> m_Size = Core::Vec2<i32>(640, 480);
    Core::Vec2<i32> m_MinSize = Core::Vec2<i32>(640, 480);
    Core::Vec2<i32> m_MaxSize = Core::Vec2<i32>(640, 480);

    virtual bool OnCreate() { return true; }
    virtual void OnDestroy() {}
    virtual void OnResize() {}
    virtual void OnLeftMouseButtonDown() {}
    virtual void OnLeftMouseButtonUp() {}
    virtual bool OnLeftMouseButtonDoubleClick() { return false; }
    virtual void OnRightMouseButtonDown() {}
    virtual void OnRightMouseButtonUp() {}
    virtual bool OnRightMouseButtonDoubleClick() { return false; }
    virtual void OnMidMouseButtonDown() {}
    virtual void OnMidMouseButtonUp() {}
    virtual bool OnMidMouseButtonDoubleClick() { return false; }
    virtual void OnMidMouseButtonScroll(bool up) {}
    virtual void OnXMouseButton(bool up) {}
    virtual void OnDragging() {}
    virtual void OnActivate() {}
    virtual void OnDeactivate() {}
    virtual void OnShow(bool show) {}
    virtual void OnTimer(u32 id) {}
    virtual void OnSetText(const char *text) {}
    virtual bool OnRepaint(const PaintEvent &ev);
    virtual bool OnUserMessages(const UserEvent &ev) { return true; }
    virtual void OnTextInput(const TextEvent &ev) {}
    virtual void OnKeyDown(const KeyEvent &ev) {}
    virtual void OnKeyUp(const KeyEvent &ev) {}
};

extern CWindow *g_WispWindow;

}; // namespace Wisp

#endif //WISP_WIN_H
