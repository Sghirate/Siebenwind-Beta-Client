#include "SDL2Input.h"
#include "Core/Log.h"
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

EScanCode SDL2Keyboard::SDLScanCodeToCoreScanCode(i32 a_scanCode)
{
    // SDL should be a direct 1:1 mapping!
    return (EScanCode)a_scanCode;
}

i32 SDL2Keyboard::CoreScanCodeToSDLScanCode(EScanCode a_scanCode)
{
    return (i32)a_scanCode;
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

EGamepadAxis SDL2Gamepad::SDLAxisToCoreAxis(int a_axis)
{
    switch(a_axis)
    {
    case SDL_CONTROLLER_AXIS_LEFTX: return EGamepadAxis::LeftX;
    case SDL_CONTROLLER_AXIS_LEFTY: return EGamepadAxis::LeftY;
    case SDL_CONTROLLER_AXIS_RIGHTX: return EGamepadAxis::RightX;
    case SDL_CONTROLLER_AXIS_RIGHTY: return EGamepadAxis::RightY;
    case SDL_CONTROLLER_AXIS_TRIGGERLEFT: return EGamepadAxis::LeftTrigger;
    case SDL_CONTROLLER_AXIS_TRIGGERRIGHT: return EGamepadAxis::RightTrigger;
    case SDL_CONTROLLER_AXIS_MAX: return EGamepadAxis::COUNT;
    default: return EGamepadAxis::Unknown;
    }
}

int SDL2Gamepad::CoreAxisToSDLAxis(EGamepadAxis a_axis)
{
    switch(a_axis)
    {
    case EGamepadAxis::LeftX: return SDL_CONTROLLER_AXIS_LEFTX;
    case EGamepadAxis::LeftY: return SDL_CONTROLLER_AXIS_LEFTY;
    case EGamepadAxis::RightX: return SDL_CONTROLLER_AXIS_RIGHTX;
    case EGamepadAxis::RightY: return SDL_CONTROLLER_AXIS_RIGHTY;
    case EGamepadAxis::LeftTrigger: return SDL_CONTROLLER_AXIS_TRIGGERLEFT;
    case EGamepadAxis::RightTrigger: return SDL_CONTROLLER_AXIS_TRIGGERRIGHT;
    case EGamepadAxis::COUNT: return SDL_CONTROLLER_AXIS_MAX;
    default: return SDL_CONTROLLER_AXIS_INVALID;
    }
}

EGamepadButton SDL2Gamepad::SDLButtonToCoreButton(int a_button)
{
    switch (a_button)
    {
    case SDL_CONTROLLER_BUTTON_A: return EGamepadButton::A;
    case SDL_CONTROLLER_BUTTON_B: return EGamepadButton::B;
    case SDL_CONTROLLER_BUTTON_X: return EGamepadButton::X;
    case SDL_CONTROLLER_BUTTON_Y: return EGamepadButton::Y;
    case SDL_CONTROLLER_BUTTON_BACK: return EGamepadButton::Back;
    case SDL_CONTROLLER_BUTTON_GUIDE: return EGamepadButton::Guide;
    case SDL_CONTROLLER_BUTTON_START: return EGamepadButton::Start;
    case SDL_CONTROLLER_BUTTON_LEFTSTICK: return EGamepadButton::LeftStick;
    case SDL_CONTROLLER_BUTTON_RIGHTSTICK: return EGamepadButton::RightStick;
    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: return EGamepadButton::LeftShoulder;
    case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return EGamepadButton::RightShoulder;
    case SDL_CONTROLLER_BUTTON_DPAD_UP: return EGamepadButton::DPadUp;
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN: return EGamepadButton::DPadDown;
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT: return EGamepadButton::DPadLeft;
    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: return EGamepadButton::DPadRight;
    case SDL_CONTROLLER_BUTTON_MISC1: return EGamepadButton::Misc1;
    case SDL_CONTROLLER_BUTTON_PADDLE1: return EGamepadButton::Paddle1;
    case SDL_CONTROLLER_BUTTON_PADDLE2: return EGamepadButton::Paddle2;
    case SDL_CONTROLLER_BUTTON_PADDLE3: return EGamepadButton::Paddle3;
    case SDL_CONTROLLER_BUTTON_PADDLE4: return EGamepadButton::Paddle4;
    case SDL_CONTROLLER_BUTTON_TOUCHPAD: return EGamepadButton::TouchPad;
    case SDL_CONTROLLER_BUTTON_MAX: return EGamepadButton::COUNT;
    default: return EGamepadButton::Unknown;
    }
}

int SDL2Gamepad::CoreButtonToSDLButton(EGamepadButton a_button)
{
    switch (a_button)
    {
    case EGamepadButton::A: return SDL_CONTROLLER_BUTTON_A;
    case EGamepadButton::B: return SDL_CONTROLLER_BUTTON_B;
    case EGamepadButton::X: return SDL_CONTROLLER_BUTTON_X;
    case EGamepadButton::Y: return SDL_CONTROLLER_BUTTON_Y;
    case EGamepadButton::Back: return SDL_CONTROLLER_BUTTON_BACK;
    case EGamepadButton::Guide: return SDL_CONTROLLER_BUTTON_GUIDE;
    case EGamepadButton::Start: return SDL_CONTROLLER_BUTTON_START;
    case EGamepadButton::LeftStick: return SDL_CONTROLLER_BUTTON_LEFTSTICK;
    case EGamepadButton::RightStick: return SDL_CONTROLLER_BUTTON_RIGHTSTICK;
    case EGamepadButton::LeftShoulder: return SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
    case EGamepadButton::RightShoulder: return SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
    case EGamepadButton::DPadUp: return SDL_CONTROLLER_BUTTON_DPAD_UP;
    case EGamepadButton::DPadDown: return SDL_CONTROLLER_BUTTON_DPAD_DOWN;
    case EGamepadButton::DPadLeft: return SDL_CONTROLLER_BUTTON_DPAD_LEFT;
    case EGamepadButton::DPadRight: return SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
    case EGamepadButton::Misc1: return SDL_CONTROLLER_BUTTON_MISC1;
    case EGamepadButton::Paddle1: return SDL_CONTROLLER_BUTTON_PADDLE1;
    case EGamepadButton::Paddle2: return SDL_CONTROLLER_BUTTON_PADDLE2;
    case EGamepadButton::Paddle3: return SDL_CONTROLLER_BUTTON_PADDLE3;
    case EGamepadButton::Paddle4: return SDL_CONTROLLER_BUTTON_PADDLE4;
    case EGamepadButton::TouchPad: return SDL_CONTROLLER_BUTTON_TOUCHPAD;
    case EGamepadButton::COUNT: return SDL_CONTROLLER_BUTTON_MAX;
    default: return SDL_CONTROLLER_BUTTON_INVALID;
    }
}

void SDL2Gamepad::Reset()
{
    for (int i = 0; i < (int)EGamepadStick::COUNT; ++i)
        sticks[i].set(0, 0);
    for (int i = 0; i < (int)EGamepadTrigger::COUNT; ++i)
        triggers[i] = 0;
    buttons.Reset();
    if (handle)
    {
        SDL_GameControllerClose(static_cast<SDL_GameController*>(handle));
        handle = nullptr;
    }
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
    m_gamepadListeners.reserve(8);

    if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) < 0)
    {
        LOG_ERROR("Core::SDL2", "Could not initialize game controller subsystem! (%s)", SDL_GetError());
    }
    SDL_GameControllerEventState(SDL_ENABLE);
}

void SDL2Input::Shutdown()
{
    SDL_GameControllerEventState(SDL_DISABLE);
    SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);

    m_mouseListeners.clear();
    m_keyboardListeners.clear();
    m_gamepadListeners.clear();

}

void SDL2Input::HandleEvent(SDL_Event* a_event)
{
    switch (a_event->type)
    {
        case SDL_MOUSEMOTION:
        {
            m_mouse.m_position.x = a_event->motion.x;
            m_mouse.m_position.y = a_event->motion.y;
            m_mouse.m_focus = a_event->motion.windowID;

            MouseEvent mouseEvent {};
            mouseEvent.mouseIndex = a_event->motion.which;
            mouseEvent.focus = SDL2Window::GetWindow(m_mouse.m_focus);
            mouseEvent.type = EMouseEventType::Motion;
            mouseEvent.pos = m_mouse.m_position;

            for (IMouseListener* listener : m_mouseListeners)
                listener->OnMouseEvent(mouseEvent);
        } break;
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
        {
            m_mouse.m_position.x = a_event->button.x;
            m_mouse.m_position.y = a_event->button.y;
            m_mouse.m_focus = a_event->button.windowID;

            MouseEvent mouseEvent {};
            mouseEvent.mouseIndex = a_event->motion.which;
            mouseEvent.focus = SDL2Window::GetWindow(m_mouse.m_focus);
            mouseEvent.type = EMouseEventType::Button;
            mouseEvent.pos = m_mouse.m_position;
            mouseEvent.button.button = SDL2Mouse::SDLButtonToCoreButton(a_event->button.button);
            mouseEvent.button.clicks = a_event->button.clicks;
            mouseEvent.button.state = a_event->button.state == SDL_PRESSED;
            u8 buttonMask = static_cast<u8>(mouseEvent.button.button);
            if (a_event->button.state == SDL_PRESSED)
                m_mouse.m_buttons |= buttonMask;
            else
                m_mouse.m_buttons &=- ~buttonMask;

            for (IMouseListener* listener : m_mouseListeners)
                listener->OnMouseEvent(mouseEvent);
        } break;
        case SDL_MOUSEWHEEL:
        {
            m_mouse.m_focus = a_event->motion.windowID;

            MouseEvent mouseEvent {};
            mouseEvent.mouseIndex = a_event->motion.which;
            mouseEvent.focus = SDL2Window::GetWindow(m_mouse.m_focus);
            mouseEvent.type = EMouseEventType::Wheel;
            mouseEvent.pos = m_mouse.m_position;
            mouseEvent.wheel.delta = TMouseWheelDelta(
                a_event->wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? (-a_event->wheel.x) : a_event->wheel.x,
                a_event->wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? (-a_event->wheel.y) : a_event->wheel.y);
            m_mouse.m_wheelDelta.x += mouseEvent.wheel.delta.x;
            m_mouse.m_wheelDelta.y += mouseEvent.wheel.delta.y;

            for (IMouseListener* listener : m_mouseListeners)
                listener->OnMouseEvent(mouseEvent);
        } break;

        case SDL_KEYDOWN:
        case SDL_KEYUP:
        {
            m_keyboard.m_focus = a_event->key.windowID;

            KeyEvent keyEvent {};
            keyEvent.focus = SDL2Window::GetWindow(m_keyboard.m_focus);
            keyEvent.scancode = SDL2Keyboard::SDLScanCodeToCoreScanCode(a_event->key.keysym.scancode);
            keyEvent.key = SDL2Keyboard::SDLKeyToCoreKey(a_event->key.keysym.sym);
            //keyEvent.mod = EModifier::None;
            keyEvent.repeat = a_event->key.repeat;
            keyEvent.state = a_event->key.state == SDL_PRESSED;

            for (IKeyboardListener* listener : m_keyboardListeners)
                listener->OnKeyboardEvent(keyEvent);
        } break;

        case SDL_TEXTINPUT:
        {
            m_keyboard.m_focus = a_event->text.windowID;

            TextEvent textEvent {};
            textEvent.focus = SDL2Window::GetWindow(m_keyboard.m_focus);
            textEvent.length = SDL_TEXTINPUTEVENT_TEXT_SIZE;
            textEvent.text = a_event->text.text;

            for (IKeyboardListener* listener : m_keyboardListeners)
                listener->OnTextEvent(textEvent);
        } break;
        
        case SDL_CONTROLLERAXISMOTION:
        {
            GamepadEvent gamepadEvent {};
            gamepadEvent.type = EGamepadEventType::Axis;
            gamepadEvent.gamepadIndex = (u8)a_event->caxis.which;
            gamepadEvent.axis.axis = SDL2Gamepad::SDLAxisToCoreAxis(a_event->caxis.axis);
            gamepadEvent.axis.value = (float)((double)a_event->caxis.value / (double)INT16_MAX);
            if (gamepadEvent.gamepadIndex >= SDL2Input::kMaxGamepads)
                return;

            SDL2Gamepad& gamepad = SDL2Input::GetGamepad(gamepadEvent.gamepadIndex);
            switch(gamepadEvent.axis.axis)
            {
            case EGamepadAxis::LeftX: gamepad.sticks[(size_t)EGamepadStick::Left].x = gamepadEvent.axis.value; break;
            case EGamepadAxis::LeftY: gamepad.sticks[(size_t)EGamepadStick::Left].y = gamepadEvent.axis.value; break;
            case EGamepadAxis::RightX: gamepad.sticks[(size_t)EGamepadStick::Right].x = gamepadEvent.axis.value; break;
            case EGamepadAxis::RightY: gamepad.sticks[(size_t)EGamepadStick::Right].y = gamepadEvent.axis.value; break;
            case EGamepadAxis::LeftTrigger: gamepad.triggers[(size_t)EGamepadStick::Left] = gamepadEvent.axis.value; break;
            case EGamepadAxis::RightTrigger: gamepad.triggers[(size_t)EGamepadStick::Right] = gamepadEvent.axis.value; break;
            default: return; // Invalid/Unknown axis!
            }

            for (IGamepadListener* listener : m_gamepadListeners)
                listener->OnGamepadEvent(gamepadEvent);
        } break;
        case SDL_CONTROLLERBUTTONDOWN:
        case SDL_CONTROLLERBUTTONUP:
        {
            GamepadEvent gamepadEvent {};
            gamepadEvent.type = EGamepadEventType::Button;
            gamepadEvent.gamepadIndex = a_event->cbutton.which;
            gamepadEvent.button.button = SDL2Gamepad::SDLButtonToCoreButton(a_event->cbutton.button);
            gamepadEvent.button.state = a_event->cbutton.state == SDL_PRESSED;
            if (gamepadEvent.gamepadIndex >= SDL2Input::kMaxGamepads ||
                gamepadEvent.button.button <= EGamepadButton::Unknown ||
                gamepadEvent.button.button >= EGamepadButton::COUNT)
                return;

            SDL2Gamepad& gamepad = SDL2Input::GetGamepad(gamepadEvent.gamepadIndex);
            gamepad.buttons.Set((size_t)gamepadEvent.button.button, gamepadEvent.button.state);

            for (IGamepadListener* listener : m_gamepadListeners)
                listener->OnGamepadEvent(gamepadEvent);
        } break;
        case SDL_CONTROLLERDEVICEADDED:
        {
            GamepadEvent gamepadEvent {};
            gamepadEvent.type = EGamepadEventType::Device;
            gamepadEvent.gamepadIndex = a_event->cdevice.which;
            if (gamepadEvent.gamepadIndex >= SDL2Input::kMaxGamepads)
                return;

            SDL2Gamepad& gamepad = SDL2Input::GetGamepad(gamepadEvent.gamepadIndex);
            gamepad.Reset();
            gamepad.handle = SDL_GameControllerOpen(gamepadEvent.gamepadIndex);
            if (!gamepad.handle)
                return;

            gamepadEvent.device.typestate = EGamepadDeviceState::Added;

            for (IGamepadListener* listener : m_gamepadListeners)
                listener->OnGamepadEvent(gamepadEvent);
        } break;
        case SDL_CONTROLLERDEVICEREMOVED:
        {
            GamepadEvent gamepadEvent {};
            gamepadEvent.type = EGamepadEventType::Device;
            gamepadEvent.gamepadIndex = a_event->cdevice.which;
            if (gamepadEvent.gamepadIndex >= SDL2Input::kMaxGamepads)
                return;

            SDL2Gamepad& gamepad = SDL2Input::GetGamepad(gamepadEvent.gamepadIndex);
            gamepad.Reset();

            gamepadEvent.device.typestate = EGamepadDeviceState::Removed;

            for (IGamepadListener* listener : m_gamepadListeners)
                listener->OnGamepadEvent(gamepadEvent);
        } break;
        case SDL_CONTROLLERDEVICEREMAPPED:
        {
            GamepadEvent gamepadEvent {};
            gamepadEvent.type = EGamepadEventType::Device;
            gamepadEvent.gamepadIndex = a_event->cdevice.which;
            if (gamepadEvent.gamepadIndex >= SDL2Input::kMaxGamepads)
                return;

            // TODO: handle: SDL2Gamepad& gamepad = SDL2Input::GetGamepad(gamepadEvent.gamepadIndex);

            gamepadEvent.device.typestate = EGamepadDeviceState::Remapped;

            for (IGamepadListener* listener : m_gamepadListeners)
                listener->OnGamepadEvent(gamepadEvent);
        } break;
        // case SDL_CONTROLLERTOUCHPADDOWN:
        // case SDL_CONTROLLERTOUCHPADMOTION:
        // case SDL_CONTROLLERTOUCHPADUP:
        // case SDL_CONTROLLERSENSORUPDATE:
        default:
            break;
    }
}

void Input::Init() { SDL2Input::Get().Init(); }
void Input::Shutdown() { SDL2Input::Get().Shutdown(); }
IMouse* Input::GetMouse(u8 a_index) { return &SDL2Input::Get().GetMouse(); }
IKeyboard* Input::GetKeyboard(u8 a_index) { return &SDL2Input::Get().GetKeyboard(); }
IGamepad* Input::GetGamepad(u8 a_index) { return a_index < SDL2Input::kMaxGamepads ? &SDL2Input::Get().GetGamepad(a_index) : nullptr; }
void Input::RegisterMouseListener(IMouseListener* a_listener) { SDL2Input::Get().RegisterMouseListener(a_listener); }
void Input::UnregisterMouseListener(IMouseListener* a_listener) { SDL2Input::Get().UnregisterMouseListener(a_listener); }
void Input::RegisterKeyboardListener(IKeyboardListener* a_listener) { SDL2Input::Get().RegisterKeyboardListener(a_listener); }
void Input::UnregisterKeyboardListener(IKeyboardListener* a_listener) { SDL2Input::Get().UnregisterKeyboardListener(a_listener); }
void Input::RegisterGamepadListener(IGamepadListener* a_listener) { SDL2Input::Get().RegisterGamepadListener(a_listener); }
void Input::UnregisterGamepadListener(IGamepadListener* a_listener) { SDL2Input::Get().UnregisterGamepadListener(a_listener); }

} // namespace Core
