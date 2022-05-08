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

struct SDL2Input
{
    static SDL2Input& Get();

    void Init();
    void Shutdown();

    void HandleEvent(SDL_Event* a_event);
    SDL2Mouse& GetMouse() { return m_mouse; }
    const SDL2Mouse& GetMouse() const { return m_mouse; }
    SDL2Keyboard& GetKeyboard() { return m_keyboard; }
    const SDL2Keyboard& GetKeyboard() const { return m_keyboard; }

    void RegisterMouseListener(IMouseListener* a_listener) { m_mouseListeners.push_back(a_listener); }
    void UnregisterMouseListener(IMouseListener* a_listener) { m_mouseListeners.erase(std::remove(m_mouseListeners.begin(), m_mouseListeners.end(), a_listener), m_mouseListeners.end()); }
    void RegisterKeyboardListener(IKeyboardListener* a_listener) { m_keyboardListeners.push_back(a_listener); }
    void UnregisterKeyboardListener(IKeyboardListener* a_listener) { m_keyboardListeners.erase(std::remove(m_keyboardListeners.begin(), m_keyboardListeners.end(), a_listener), m_keyboardListeners.end()); }

private:
    SDL2Mouse m_mouse;
    SDL2Keyboard m_keyboard;
    std::vector<IMouseListener*> m_mouseListeners;
    std::vector<IKeyboardListener*> m_keyboardListeners;
};

} // namespace Core
