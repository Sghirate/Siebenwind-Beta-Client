#pragma once

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

    Button_Left = Button_0,
    Button_Right = Button_1,
    Button_Middle = Button_2,
};
struct IMouse
{
    virtual ~IMouse() {}

    virtual u32 GetFocus() const;
    virtual bool IsButtonDown(EMouseButton a_button) const;
    virtual TMousePos GetPosition() const;
    virtual TMouseWheelDelta GetwheelDelta() const;
    virtual void SetPosition(const TMousePos& a_position);
};
struct IMouseListener
{
    virtual void OnMouseButton(u8 a_mouseIndex, const TMousePos& a_pos, EMouseButton a_button, bool a_state, u8 a_clicks) {}
    virtual void OnMouseWheel(u8 a_mouseIndex, const TMousePos& a_pos, const TMouseWheelDelta& a_delta) {}
};
// ~MOUSE

// KEYBOARD
enum class EKey : u8
{
    Key_None         = 0,
    Key_Space        = 1,
    Key_Apostrophe   = 2,
    Key_Comma        = 3,
    Key_Minus        = 4,
    Key_Period       = 5,
    Key_Slash        = 6,
    Key_0            = 7,
    Key_1            = 8,
    Key_2            = 9,
    Key_3            = 10,
    Key_4            = 11,
    Key_5            = 12,
    Key_6            = 13,
    Key_7            = 14,
    Key_8            = 15,
    Key_9            = 16,
    Key_Semicolon    = 17,
    Key_Equal        = 18,
    Key_A            = 19,
    Key_B            = 20,
    Key_C            = 21,
    Key_D            = 22,
    Key_E            = 23,
    Key_F            = 24,
    Key_G            = 25,
    Key_H            = 26,
    Key_I            = 27,
    Key_J            = 28,
    Key_K            = 29,
    Key_L            = 30,
    Key_M            = 31,
    Key_N            = 32,
    Key_O            = 33,
    Key_P            = 34,
    Key_Q            = 35,
    Key_R            = 36,
    Key_S            = 37,
    Key_T            = 38,
    Key_U            = 39,
    Key_V            = 40,
    Key_W            = 41,
    Key_X            = 42,
    Key_Y            = 43,
    Key_Z            = 44,
    Key_LeftBracket  = 45,
    Key_Backslash    = 46,
    Key_RightBracket = 47,
    Key_GraveAccent  = 48,
    Key_World1       = 49,
    Key_World2       = 50,
    Key_Escape       = 51,
    Key_Enter        = 52,
    Key_Tab          = 53,
    Key_Backspace    = 54,
    Key_Insert       = 55,
    Key_Delete       = 56,
    Key_Right        = 57,
    Key_Left         = 58,
    Key_Down         = 59,
    Key_Up           = 60,
    Key_PageUp       = 61,
    Key_PageDown     = 62,
    Key_Home         = 63,
    Key_End          = 64,
    Key_CapsLock     = 65,
    Key_ScrollLock   = 66,
    Key_NumLock      = 67,
    Key_PrintScreen  = 68,
    Key_Pause        = 69,
    Key_F1           = 70,
    Key_F2           = 71,
    Key_F3           = 72,
    Key_F4           = 73,
    Key_F5           = 74,
    Key_F6           = 75,
    Key_F7           = 76,
    Key_F8           = 77,
    Key_F9           = 78,
    Key_F10          = 79,
    Key_F11          = 80,
    Key_F12          = 81,
    Key_F13          = 82,
    Key_F14          = 83,
    Key_F15          = 84,
    Key_F16          = 85,
    Key_F17          = 86,
    Key_F18          = 87,
    Key_F19          = 88,
    Key_F20          = 89,
    Key_F21          = 90,
    Key_F22          = 91,
    Key_F23          = 92,
    Key_F24          = 93,
    Key_F25          = 94,
    Key_Kp0          = 95,
    Key_Kp1          = 96,
    Key_Kp2          = 97,
    Key_Kp3          = 98,
    Key_Kp4          = 99,
    Key_Kp5          = 100,
    Key_Kp6          = 101,
    Key_Kp7          = 102,
    Key_Kp8          = 103,
    Key_Kp9          = 104,
    Key_KpDecimal    = 105,
    Key_KpDivide     = 106,
    Key_KpMultiply   = 107,
    Key_KpSubtract   = 108,
    Key_KpAdd        = 109,
    Key_KpEnter      = 110,
    Key_KpEqual      = 111,
    Key_LeftShift    = 112,
    Key_LeftControl  = 113,
    Key_LeftAlt      = 114,
    Key_LeftSuper    = 115,
    Key_RightShift   = 116,
    Key_RightControl = 117,
    Key_RightAlt     = 118,
    Key_RightSuper   = 119,
    Key_Menu         = 120,

    Key_COUNT
};
struct IKeyboard
{
    virtual ~IKeyboard() {}


};
struct IKeyboardListener
{

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

    static void RegisterMouseListener(IMouseListener* a_listener);
    static void UnregisterMouseListener(IMouseListener* a_listener);
    static void RegisterKeyboardListener(IKeyboardListener* a_listener);
    static void UnregisterKeyboardListener(IKeyboardListener* a_listener);
};

} // namespace Core
