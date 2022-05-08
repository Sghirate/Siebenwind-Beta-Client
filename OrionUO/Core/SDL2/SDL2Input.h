#pragma once

#include "Core/BitSet.h"
#include "Core/Input.h"
#include <algorithm>
#include <vector>

union SDL_Event;

namespace Core
{

struct SDL2Mouse : public IMouse
{
    static EMouseButton SDLButtonToCoreButton(u8 a_button);
    static u8 CoreButtonToSDLButton(EMouseButton a_button);

    SDL2Mouse() {}
    ~SDL2Mouse() {}

    // IMouse
    bool IsButtonDown(EMouseButton a_button) const override { return (m_buttons & (1 << static_cast<u8>(a_button))) == (1 << static_cast<u8>(a_button)); }
    TMousePos GetPosition() const override { return m_position; }
    TMouseWheelDelta GetwheelDelta() const override { return m_wheelDelta; }
    void SetPosition(const TMousePos& a_position) override { m_position = a_position; }
    // ~IMouse

    u32 GetFocus() const override { return m_focus; }

public:
    u32 m_focus = 0;
    u8 m_buttons = 0;
    TMousePos m_position;
    TMouseWheelDelta m_wheelDelta;
};
struct SDL2Keyboard : public IKeyboard
{
    static EScanCode SDLScanCodeToCoreScanCode(i32 a_scanCode);
    static i32 CoreScanCodeToSDLScanCode(EScanCode a_scanCode);
    static EKey SDLKeyToCoreKey(i32 a_key);
    static i32 CoreKeyToSDLKey(EKey a_key);

    SDL2Keyboard() {}
    ~SDL2Keyboard() {}

    u32 GetFocus() const { return m_focus; }

public:
    u32 m_focus = 0;
    BitSet<128> m_keys;
};

struct SDL2Gamepad : public IGamepad
{
    static EGamepadAxis SDLAxisToCoreAxis(int a_axis);
    static int CoreAxisToSDLAxis(EGamepadAxis a_axis);
    static EGamepadButton SDLButtonToCoreButton(int a_button);
    static int CoreButtonToSDLButton(EGamepadButton a_button);

    SDL2Gamepad() { Reset(); }
    ~SDL2Gamepad() {}

    void Reset();

    // IGamepad
    Vec2<float> GetStickValues(EGamepadStick a_stick) const override { return sticks[(size_t)a_stick]; }
    float GetTriggerValue(EGamepadTrigger a_trigger) const override { return triggers[(size_t)a_trigger]; }
    bool IsButtonPressed(EGamepadButton a_button) const override { return buttons.Get((size_t)a_button); }
    // ~IGamepad

public:
    Vec2<float> sticks[(size_t)EGamepadStick::COUNT];
    float triggers[(size_t)EGamepadTrigger::COUNT];
    BitSet<(size_t)EGamepadButton::COUNT> buttons;
    void* handle = nullptr;
};

struct SDL2Input
{
    enum { kMaxGamepads = 8 };

    static SDL2Input& Get();

    void Init();
    void Shutdown();

    void HandleEvent(SDL_Event* a_event);
    SDL2Mouse& GetMouse() { return m_mouse; }
    const SDL2Mouse& GetMouse() const { return m_mouse; }
    SDL2Keyboard& GetKeyboard() { return m_keyboard; }
    const SDL2Keyboard& GetKeyboard() const { return m_keyboard; }
    SDL2Gamepad& GetGamepad(u8 a_gamepadIndex) { return m_gamepads[a_gamepadIndex]; }
    const SDL2Gamepad& GetGamepad(u8 a_gamepadIndex) const { return m_gamepads[a_gamepadIndex]; }

    void RegisterMouseListener(IMouseListener* a_listener) { m_mouseListeners.push_back(a_listener); }
    void UnregisterMouseListener(IMouseListener* a_listener) { m_mouseListeners.erase(std::remove(m_mouseListeners.begin(), m_mouseListeners.end(), a_listener), m_mouseListeners.end()); }
    void RegisterKeyboardListener(IKeyboardListener* a_listener) { m_keyboardListeners.push_back(a_listener); }
    void UnregisterKeyboardListener(IKeyboardListener* a_listener) { m_keyboardListeners.erase(std::remove(m_keyboardListeners.begin(), m_keyboardListeners.end(), a_listener), m_keyboardListeners.end()); }
    void RegisterGamepadListener(IGamepadListener* a_listener) { m_gamepadListeners.push_back(a_listener); }
    void UnregisterGamepadListener(IGamepadListener* a_listener) { m_gamepadListeners.erase(std::remove(m_gamepadListeners.begin(), m_gamepadListeners.end(), a_listener), m_gamepadListeners.end()); }

private:
    SDL2Mouse m_mouse;
    SDL2Keyboard m_keyboard;
    SDL2Gamepad m_gamepads[kMaxGamepads];
    std::vector<IMouseListener*> m_mouseListeners;
    std::vector<IKeyboardListener*> m_keyboardListeners;
    std::vector<IGamepadListener*> m_gamepadListeners;
};

} // namespace Core
