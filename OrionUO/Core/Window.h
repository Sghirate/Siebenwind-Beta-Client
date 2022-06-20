#pragma once

#include "Core/Minimal.h"
#include <memory>

namespace Core
{

using TWindowSize = Vec2<i32>;
using TWindowPosition = Vec2<i32>;
struct Window
{
    Window();
    virtual ~Window();

    bool Create(const char* a_title, const TWindowPosition& a_initialPosition, const TWindowSize& a_initialSize);
    void* GetHandle() const { return m_handle; }
    void SetHandle(void* a_handle) { m_handle = a_handle; }
    bool IsOpen() const { return m_handle != nullptr; }

    TWindowSize GetSize() const;
    void SetSize(const TWindowSize& a_size);
    TWindowSize GetMinSize() const;
    void SetMinSize(const TWindowSize& a_size);
    TWindowSize GetMaxSize() const;
    void SetMaxSize(const TWindowSize& a_size);
    TWindowPosition GetPosition() const;
    void SetPosition(const TWindowPosition& a_position);
    const char* GetTitle() const;
    void SetTitle(const char* a_title);
    void SetIsResizeable(bool a_resizeable);
    bool IsActive() const;
    bool HasMouseFocus() const;
    bool HasKeyboardFocus() const;
    bool IsMinimized() const;
    void Minimize();
    bool IsMaximized() const;
    void Maximize();
    bool IsBorderless() const;
    void SetBorderless(bool a_borderless);
    bool IsVisible() const;
    void Show();
    void Hide();
    void BringToFront();
    void Restore();
    void Close();
    void Destroy();

    virtual void OnCreated();
    virtual void OnDestroy();
    virtual void OnResized();
    virtual void OnDragging();
    virtual void OnActivation(bool a_isActive);
    virtual void OnVisibility(bool a_isVisible);

private:
    void* m_handle;
};

} // namespace Core
