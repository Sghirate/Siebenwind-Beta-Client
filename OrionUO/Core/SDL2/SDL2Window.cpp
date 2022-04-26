#include "Core/SDL2/SDL2Window.h"
#include "Core/SDL2/SDL2Input.h"
#include "Core/SDL2/SDL2Util.h"
#include "Core/Log.h"
#include <SDL.h>
#include <list>

namespace Core
{

static const char* CORE_WINDOW_DATA_NAME = "core_window";

Window::Window()
     : m_handle(nullptr)
{
}

Window::~Window()
{
}

bool Window::Create(
    const char* a_title, const TWindowSize& a_initialPosition, const TWindowSize& a_initialSize)
{
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
    {
        LOG_FATAL("Core::SDL2", "Could not initialize video subsystem!");
        return false;
    }

    SDL_Window* sdlWindow = SDL_CreateWindow(
        a_title,
        a_initialPosition.x,
        a_initialPosition.y,
        a_initialSize.x,
        a_initialSize.y,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!sdlWindow)
    {
        LOG_FATAL("Core::SDL2", "Could not create window!");
        return false;
    }
    m_handle = sdlWindow;
    SDL_SetWindowData(sdlWindow, CORE_WINDOW_DATA_NAME, this);
    OnCreated();
    return true;
}

TWindowSize Window::GetSize() const
{
    int x = 0;
    int y = 0;
    if (m_handle)
        SDL_GetWindowSize(static_cast<SDL_Window*>(m_handle), &x, &y);
    return TWindowSize(x, y);
}

void Window::SetSize(const TWindowSize& a_size)
{
    if (m_handle)
        SDL_SetWindowSize(static_cast<SDL_Window*>(m_handle), a_size.x, a_size.y);
}

TWindowSize Window::GetMinSize() const
{
    int x = -1;
    int y = -1;
    if (m_handle)
        SDL_GetWindowMinimumSize(static_cast<SDL_Window*>(m_handle), &x, &y);
    return TWindowSize(x, y);
}

void Window::SetMinSize(const TWindowSize& a_size)
{
    if (m_handle)
        SDL_SetWindowMinimumSize(static_cast<SDL_Window*>(m_handle), a_size.x, a_size.y);
}

TWindowSize Window::GetMaxSize() const
{
    int x = -1;
    int y = -1;
    if (m_handle)
        SDL_GetWindowMaximumSize(static_cast<SDL_Window*>(m_handle), &x, &y);
    return TWindowSize(x, y);
}

void Window::SetMaxSize(const TWindowSize& a_size)
{
    if (m_handle)
        SDL_SetWindowMaximumSize(static_cast<SDL_Window*>(m_handle), a_size.x, a_size.y);
}

TWindowPosition Window::GetPosition() const
{
    int x = -1;
    int y = -1;
    if (m_handle)
        SDL_GetWindowPosition(static_cast<SDL_Window*>(m_handle), &x, &y);
    return TWindowPosition(x, y);
}

void Window::SetPosition(const TWindowPosition& a_position)
{
    if (m_handle)
        SDL_SetWindowPosition(static_cast<SDL_Window*>(m_handle), a_position.x, a_position.y);
}

const char* Window::GetTitle() const
{
    return m_handle ? SDL_GetWindowTitle(static_cast<SDL_Window*>(m_handle)) : nullptr;
}

void Window::SetTitle(const char* a_title)
{
    if (m_handle)
        SDL_SetWindowTitle(static_cast<SDL_Window*>(m_handle), a_title);
}

void Window::SetIsResizeable(bool a_resizeable)
{
    if (m_handle)
        SDL_SetWindowResizable(static_cast<SDL_Window*>(m_handle), a_resizeable ? SDL_TRUE : SDL_FALSE);
}

bool Window::IsActive() const
{
    return m_handle ? (SDL_GetWindowFlags(static_cast<SDL_Window*>(m_handle)) & SDL_WINDOW_INPUT_FOCUS) == SDL_WINDOW_INPUT_FOCUS : false;
}

bool Window::HasMouseFocus() const
{
    return false;
    // TODO: implement
}

bool Window::HasKeyboardFocus() const
{
    return false;
    // TODO: implement
}

bool Window::IsMinimized() const
{
    return m_handle ? (SDL_GetWindowFlags(static_cast<SDL_Window*>(m_handle)) & SDL_WINDOW_MINIMIZED) == SDL_WINDOW_MINIMIZED : false;
}

void Window::Minimize()
{
    if (m_handle)
        SDL_MinimizeWindow(static_cast<SDL_Window*>(m_handle));
}

bool Window::IsMaximized() const
{
    return m_handle ? (SDL_GetWindowFlags(static_cast<SDL_Window*>(m_handle)) & SDL_WINDOW_MAXIMIZED) == SDL_WINDOW_MAXIMIZED : false;
}

void Window::Maximize()
{
    if (m_handle)
        SDL_MaximizeWindow(static_cast<SDL_Window*>(m_handle));
}

bool Window::IsBorderless() const
{
    return m_handle ? (SDL_GetWindowFlags(static_cast<SDL_Window*>(m_handle)) & SDL_WINDOW_BORDERLESS) == SDL_WINDOW_BORDERLESS : false;
}

void Window::SetBorderless(bool a_borderless)
{
    if (m_handle)
        SDL_SetWindowBordered(static_cast<SDL_Window*>(m_handle), a_borderless ? SDL_FALSE : SDL_TRUE);
}

bool Window::IsVisible() const
{
    return m_handle ? (SDL_GetWindowFlags(static_cast<SDL_Window*>(m_handle)) & SDL_WINDOW_SHOWN) == SDL_WINDOW_SHOWN : false;
}

void Window::Show()
{
    if (m_handle)
        SDL_ShowWindow(static_cast<SDL_Window*>(m_handle));
}

void Window::Hide()
{
    if (m_handle)
        SDL_HideWindow(static_cast<SDL_Window*>(m_handle));
}

void Window::BringToFront()
{
    if (m_handle)
        SDL_RaiseWindow(static_cast<SDL_Window*>(m_handle));
}

void Window::Restore()
{
    if (m_handle)
        SDL_RestoreWindow(static_cast<SDL_Window*>(m_handle));
}

void Window::Destroy()
{
    if (m_handle)
    {
        SDL_Event event;
        SDL_zero(event);
        event.type          = SDL_USEREVENT;
        event.user.type     = SDL_USEREVENT;
        event.user.code     = SDL_QUIT;
        event.user.windowID = SDL_GetWindowID(static_cast<SDL_Window*>(m_handle));
        event.user.data1    = this;
        event.user.data2    = static_cast<SDL_Window*>(m_handle);
        SDL_PushEvent(&event);
    }
}

void Window::OnCreated()
{
    if (m_handle)
        SDL_SetWindowData(static_cast<SDL_Window*>(m_handle), CORE_WINDOW_DATA_NAME, this);
}
void Window::OnDestroy()
{
    if (m_handle)
    {
        SDL_SetWindowData(static_cast<SDL_Window*>(m_handle), CORE_WINDOW_DATA_NAME, nullptr);
        m_handle = nullptr;
    }
}
void Window::OnResized() {}
void Window::OnDragging() {}
void Window::OnActivation(bool a_isActive) {}
void Window::OnVisibility(bool a_isVisible) {}

namespace SDL2Window
{

u32 GetWindowID(Window* a_window)
{
    SDL_Window* sdlWindow = a_window ? static_cast<SDL_Window*>(a_window->GetHandle()) : nullptr;
    return sdlWindow ? SDL_GetWindowID(sdlWindow) : 0;
}

Window* GetWindow(u32 a_id)
{
    SDL_Window* sdlWindow = SDL_GetWindowFromID(a_id);
    Window* window = sdlWindow ? static_cast<Window*>(SDL_GetWindowData(sdlWindow, CORE_WINDOW_DATA_NAME)) : nullptr;
    return window;
}

void HandleEvent(union SDL_Event* a_event)
{
    if (SDL2Util::IsEventCategory(a_event, SDL_EVENTCATEGORY_QUIT))
    {
        // TODO: Handle
        
    }
    else if (SDL2Util::IsEventCategory(a_event, SDL_EVENTCATEGORY_USER))
    {
        switch (a_event->user.code)
        {
            case SDL_QUIT:
            {
                if (a_event->user.data1 && a_event->user.data2)
                {
                    Window* window = (Window*)a_event->user.data1;
                    window->OnDestroy();
                }
            }
            break;
        }
    }
    else if (SDL2Util::IsEventCategory(a_event, SDL_EVENTCATEGORY_WINDOW))
    {
        SDL_Window* sdlWindow = SDL_GetWindowFromID(a_event->window.windowID);
        Window* window = sdlWindow ? (Window*)SDL_GetWindowData(sdlWindow, CORE_WINDOW_DATA_NAME) : nullptr;
        if (window && window->GetHandle() == sdlWindow)
        {
            switch (a_event->window.event)
            {
                case SDL_WINDOWEVENT_CLOSE:
                {
                    window->OnDestroy();
                }
                break;
                case SDL_WINDOWEVENT_SHOWN:
                {
                    window->OnVisibility(true);
                    window->OnActivation(true);
                }
                break;
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                case SDL_WINDOWEVENT_RESIZED:
                {
                    window->OnResized();
                }
                break;
                case SDL_WINDOWEVENT_MAXIMIZED:
                {
                    window->OnResized();
                }
                break;
                case SDL_WINDOWEVENT_RESTORED:
                {
                    int x, y, borderH = 0;
#if SDL_VERSION_ATLEAST(2, 0, 5)
                    SDL_GetWindowBordersSize(sdlWindow, &borderH, nullptr, nullptr, nullptr);
#endif
                    SDL_GetWindowPosition(sdlWindow, &x, &y);
                    SDL_SetWindowPosition(sdlWindow, x, Core::Max(y, borderH));
                    window->OnResized();
                }
                break;

                case SDL_WINDOWEVENT_HIDDEN:
                {
                    window->OnVisibility(false);
                    window->OnActivation(false);
                }
                break;
            }
        }
    }
}

};

} // namespace Core
