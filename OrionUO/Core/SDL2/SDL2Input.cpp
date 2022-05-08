#include "SDL2Input.h"
#include "SDL2Util.h"
#include "Core/SDL2/SDL2Window.h"
#include <SDL.h>

namespace Core
{

static SDL2Input g_sdl2Input;

EMouseButton SDL2Mouse::SDLButtonToCoreButton(u8 a_button)
{
    switch(a_button)
    {
        case SDL_BUTTON_MIDDLE: return static_cast<EMouseButton>(SDL_BUTTON_RIGHT - 1);
        case SDL_BUTTON_RIGHT: return static_cast<EMouseButton>(SDL_BUTTON_MIDDLE - 1);
        default: return static_cast<EMouseButton>(a_button - 1);
    }
}
u8 SDL2Mouse::CoreButtonToSDLButton(EMouseButton a_button)
{
    switch (a_button)
    {
    case EMouseButton::Button_Middle: return static_cast<u8>(EMouseButton::Button_Right) + 1;
    case EMouseButton::Button_Right: return static_cast<u8>(EMouseButton::Button_Middle) + 1;
    default: return static_cast<u8>(a_button) + 1;
    }
}

EKey SDL2Keyboard::SDLKeyToCoreKey(i32 a_key)
{
    // SDL should be a direct 1:1 mapping!
    return (EKey)a_key;
}

i32 SDL2Keyboard::CoreKeyToSDLKey(EKey a_key)
{
    return (i32)a_key;
}

SDL2Input& SDL2Input::Get()
{
    static SDL2Input s_instance;
    return s_instance;
}

void SDL2Input::Init()
{
    m_mouseListeners.reserve(8);
    m_keyboardListeners.reserve(8);
}

void SDL2Input::Shutdown()
{
    m_mouseListeners.clear();
    m_keyboardListeners.clear();
}

void SDL2Input::HandleEvent(SDL_Event* a_event)
{
    if (SDL2Util::IsEventCategory(a_event, SDL_EVENTCATEGORY_MOUSE))
    {
        m_mouse.m_focus = a_event->motion.windowID;
        m_mouse.m_position.x = a_event->motion.x;
        m_mouse.m_position.y = a_event->motion.y;

        MouseEvent ev {};
        ev.mouseIndex = a_event->motion.which;
        ev.focus = SDL2Window::GetWindow(a_event->motion.windowID);
        ev.pos = m_mouse.m_position;

        switch (a_event->type)
        {
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
        {
            ev.type = EMouseEventType::Button;
            ev.button.button = SDL2Mouse::SDLButtonToCoreButton(a_event->button.button);
            ev.button.clicks = a_event->button.clicks;
            ev.button.state = a_event->button.state == SDL_PRESSED;
            u8 buttonMask = static_cast<u8>(ev.button.button);
            if (a_event->button.state == SDL_PRESSED)
                m_mouse.m_buttons |= buttonMask;
            else
                m_mouse.m_buttons &=- ~buttonMask;

            for (IMouseListener* listener : m_mouseListeners)
                listener->OnMouseEvent(ev);
        } break;
        case SDL_MOUSEWHEEL:
        {
            ev.type = EMouseEventType::Wheel;
            ev.wheel.delta = TMouseWheelDelta(
                a_event->wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? (-a_event->wheel.x) : a_event->wheel.x,
                a_event->wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? (-a_event->wheel.y) : a_event->wheel.y);
            m_mouse.m_wheelDelta.x += ev.wheel.delta.x;
            m_mouse.m_wheelDelta.y += ev.wheel.delta.y;

            for (IMouseListener* listener : m_mouseListeners)
                listener->OnMouseEvent(ev);
        } break;
        default: break;
        }
    }
    else if (SDL2Util::IsEventCategory(a_event, SDL_EVENTCATEGORY_KEYBOARD))
    {
        m_keyboard.m_focus = a_event->key.windowID;
        switch (a_event->type)
        {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            
            break;
        
        default:
            break;
        }
    }
}

void Input::Init() { SDL2Input::Get().Init(); }
void Input::Shutdown() { SDL2Input::Get().Shutdown(); }
IMouse* Input::GetMouse(u8 a_index) { return &SDL2Input::Get().GetMouse(); }
IKeyboard* Input::GetKeyboard(u8 a_index) { return &SDL2Input::Get().GetKeyboard(); }
IGamepad* Input::GetGamepad(u8 a_index) { return nullptr; }
void Input::RegisterMouseListener(IMouseListener* a_listener) { SDL2Input::Get().RegisterMouseListener(a_listener); }
void Input::UnregisterMouseListener(IMouseListener* a_listener) { SDL2Input::Get().UnregisterMouseListener(a_listener); }
void Input::RegisterKeyboardListener(IKeyboardListener* a_listener) { SDL2Input::Get().RegisterKeyboardListener(a_listener); }
void Input::UnregisterKeyboardListener(IKeyboardListener* a_listener) { SDL2Input::Get().UnregisterKeyboardListener(a_listener); }

} // namespace Core
