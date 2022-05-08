#pragma once

#include "Core/Minimal.h"
#include "Core/Keys.h"

namespace Core
{

// MOUSE
typedef Vec2<i32> TMousePos;
typedef Vec2<i8> TMouseWheelDelta;
enum class EMouseButton : u8
{
    Button_0 = 0,
    Button_1,
    Button_2,
    Button_3,
    Button_4,
    Button_5,
    Button_6,
    Button_7,

    COUNT,

    Button_Left   = Button_0,
    Button_Right  = Button_1,
    Button_Middle = Button_2,
};
enum class EMouseEventType
{
    Unknown = 0,
    Button,
    Wheel,
    Motion,
};
struct MouseButtonEvent
{
    EMouseButton button;
    u8 clicks;
    bool state;
};
struct MouseWheelEvent
{
    TMouseWheelDelta delta;
};
struct MouseEvent
{
    EMouseEventType type;
    u8 mouseIndex;
    struct Window* focus;
    TMousePos pos;
    union
    {
        MouseButtonEvent button;
        MouseWheelEvent wheel;
    };
};
struct IMouseListener
{
    virtual void OnMouseEvent(const MouseEvent& ev) = 0;
};
struct IMouse
{
    virtual ~IMouse() {}

    virtual u32 GetFocus() const                           = 0;
    virtual bool IsButtonDown(EMouseButton a_button) const = 0;
    virtual TMousePos GetPosition() const                  = 0;
    virtual TMouseWheelDelta GetwheelDelta() const         = 0;
    virtual void SetPosition(const TMousePos& a_position)  = 0;
};
// ~MOUSE

// KEYBOARD
struct KeyEvent
{
    struct Window* focus;
    EScanCode scancode;
    EKey key;
    EModifier mod;
    bool state;
    bool repeat;
};
struct TextEvent
{
    struct Window* focus;
    u32 length;
    char* text;
};
struct IKeyboardListener
{
    virtual void OnKeyboardEvent(const Core::KeyEvent& ev) = 0;
    virtual void OnTextEvent(const Core::TextEvent& ev) = 0;
};
struct IKeyboard
{
    virtual ~IKeyboard() {}
};
// ~KEYBOARD

// GAMEPAD
struct IGamepad
{
};
// ~GAMEPAD

struct Input
{
    static void Init();
    static void Shutdown();

    static IMouse* GetMouse(u8 a_index = 0);
    static IKeyboard* GetKeyboard(u8 a_index = 0);
    static IGamepad* GetGamepad(u8 a_index = 0);

    static const char* GetKeyName(EKey a_key) { return nullptr; } // TODO: IMPLEMENT!

    static void RegisterMouseListener(IMouseListener* a_listener);
    static void UnregisterMouseListener(IMouseListener* a_listener);
    static void RegisterKeyboardListener(IKeyboardListener* a_listener);
    static void UnregisterKeyboardListener(IKeyboardListener* a_listener);
};

} // namespace Core
