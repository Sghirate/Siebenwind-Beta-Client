#include "GameInput.h"
#include "ClickObject.h"
#include "Core/Input.h"
#include "GameWindow.h"
#include "Globals.h"
#include "Managers/ConfigManager.h"
#include "Managers/ScreenEffectManager.h"
#include "PressedObject.h"
#include "ScreenStages/BaseScreen.h"
#include "SelectedObject.h"

namespace
{

struct GameInputHandler : public Core::IMouseListener, public Core::IKeyboardListener
{
    void Register()
    {
        Core::Input::RegisterMouseListener(this);
        Core::Input::RegisterKeyboardListener(this);
    }
    void Unregister()
    {
        Core::Input::UnregisterKeyboardListener(this);
        Core::Input::UnregisterMouseListener(this);
    }

    void OnMouseEvent(const Core::MouseEvent& ev) override
    {
        if (ev.focus != &g_gameWindow)
            return;

        switch (ev.type)
        {
            case Core::EMouseEventType::Button:
                switch (ev.button.button)
                {
                    case Core::EMouseButton::Button_Left:
                        if (ev.button.state)
                        {
                            if (ev.button.clicks > 1)
                                GameInput::Get().OnLeftMouseButtonDoubleClick(ev.pos);
                            else
                                GameInput::Get().OnLeftMouseButtonDown(ev.pos);
                        }
                        else
                        {
                            GameInput::Get().OnLeftMouseButtonUp(ev.pos);
                        }
                        break;
                    case Core::EMouseButton::Button_Middle:
                        if (ev.button.state)
                        {
                            if (ev.button.clicks > 1)
                                GameInput::Get().OnMidMouseButtonDoubleClick(ev.pos);
                            else
                                GameInput::Get().OnMidMouseButtonDown(ev.pos);
                        }
                        else
                        {
                            GameInput::Get().OnMidMouseButtonUp(ev.pos);
                        }
                        break;
                    case Core::EMouseButton::Button_Right:
                        if (ev.button.state)
                        {
                            if (ev.button.clicks > 1)
                                GameInput::Get().OnRightMouseButtonDoubleClick(ev.pos);
                            else
                                GameInput::Get().OnRightMouseButtonDown(ev.pos);
                        }
                        else
                        {
                            GameInput::Get().OnRightMouseButtonUp(ev.pos);
                        }
                        break;
                    default: break;
                }
                break;
            case Core::EMouseEventType::Wheel:
                GameInput::Get().OnMidMouseButtonScroll(ev.pos, ev.wheel.delta.y);
                break;
            default: break;
        }
    }
    void OnKeyboardEvent(const Core::KeyEvent& ev) override
    {
        if (ev.focus != &g_gameWindow)
            return;

        if (ev.state)
            GameInput::Get().OnKeyDown(ev);
        else
            GameInput::Get().OnKeyUp(ev);
    }
    void OnTextEvent(const Core::TextEvent& ev) override
    {
        if (ev.focus != &g_gameWindow)
            return;

        GameInput::Get().OnText(ev);
    }
} g_gameInputHandler;

} // namespace

GameInput& GameInput::Get()
{
    static GameInput s_instance;
    return s_instance;
}

GameInput::GameInput()
{
}

GameInput::~GameInput()
{
}

void GameInput::Init()
{
    g_gameInputHandler.Register();
}

void GameInput::Shutdown()
{
    g_gameInputHandler.Unregister();
}

bool GameInput::IsInputPossible() const
{
    return g_CurrentScreen != nullptr && g_ScreenEffectManager.Mode == SEM_NONE;
}

void GameInput::OnMouseMove(const Core::Vec2<i32>& pos)
{
    static bool dragging = false;
    if (dragging && IsInputPossible())
    {
        g_CurrentScreen->OnDragging();
    }
}

void GameInput::OnLeftMouseButtonDown(const Core::Vec2<i32>& a_pos)
{
    if (IsInputPossible())
    {
        g_GeneratedMouseDown = false;
        g_CurrentScreen->SelectObject();
        g_PressedObject.InitLeft(g_SelectedObject);
        if (g_SelectedObject.Object != nullptr || g_GameState == GS_GAME)
        {
            g_CurrentScreen->OnLeftMouseButtonDown();
        }
    }
}

void GameInput::OnLeftMouseButtonUp(const Core::Vec2<i32>& a_pos)
{
    if (IsInputPossible())
    {
        g_CurrentScreen->SelectObject();

        //if ((g_SelectedObject.Object() != nullptr && g_SelectedObject.Object() == g_PressedObject.LeftObject && g_SelectedObject.Serial) || g_GameState >= GS_GAME)
        if ((g_SelectedObject.Object != nullptr && (g_SelectedObject.Serial != 0u)) ||
            g_GameState >= GS_GAME)
        {
            g_CurrentScreen->OnLeftMouseButtonUp();
            if (g_MovingFromMouse && g_PressedObject.LeftGump == nullptr)
            {
                g_AutoMoving = true;
            }
        }

        if (g_PressedObject.LeftObject != nullptr && g_PressedObject.LeftObject->IsGUI() &&
            ((CBaseGUI*)g_PressedObject.LeftObject)->Type == GOT_COMBOBOX &&
            g_PressedObject.LeftGump != nullptr)
        {
            g_PressedObject.LeftGump->WantRedraw = true;
        }

        g_PressedObject.ClearLeft();
    }
}

void GameInput::OnLeftMouseButtonDoubleClick(const Core::Vec2<i32>& a_pos)
{
    if (IsInputPossible())
    {
        g_CurrentScreen->SelectObject();
        g_PressedObject.InitLeft(g_SelectedObject);
        bool result = (g_SelectedObject.Object != nullptr &&
                       g_SelectedObject.Object == g_PressedObject.LeftObject) &&
                      g_CurrentScreen->OnLeftMouseButtonDoubleClick();

        if (result)
        {
            g_PressedObject.ClearLeft();
            g_ClickObject.Clear();
        }
    }
}

void GameInput::OnRightMouseButtonDown(const Core::Vec2<i32>& a_pos)
{
    if (IsInputPossible())
    {
        g_CurrentScreen->SelectObject();
        g_PressedObject.InitRight(g_SelectedObject);
        g_CurrentScreen->OnRightMouseButtonDown();
        if (g_SelectedObject.Gump == nullptr &&
            !(a_pos.x < g_ConfigManager.GameWindowX || a_pos.y < g_ConfigManager.GameWindowY ||
              a_pos.x > (g_ConfigManager.GameWindowX + g_ConfigManager.GameWindowWidth) ||
              a_pos.y > (g_ConfigManager.GameWindowY + g_ConfigManager.GameWindowHeight)))
        {
            g_MovingFromMouse = true;
            g_AutoMoving      = false;
        }
    }
}

void GameInput::OnRightMouseButtonUp(const Core::Vec2<i32>& a_pos)
{
    if (IsInputPossible())
    {
        g_CurrentScreen->SelectObject();
        if ((g_SelectedObject.Object != nullptr &&
             g_SelectedObject.Object == g_PressedObject.RightObject &&
             (g_SelectedObject.Serial != 0u)) ||
            g_GameState >= GS_GAME)
        {
            g_CurrentScreen->OnRightMouseButtonUp();
        }
        g_MovingFromMouse = false;
        g_PressedObject.ClearRight();
    }
}

void GameInput::OnRightMouseButtonDoubleClick(const Core::Vec2<i32>& a_pos)
{
    if (IsInputPossible())
    {
        g_CurrentScreen->SelectObject();
        g_PressedObject.InitRight(g_SelectedObject);
        bool result = (g_SelectedObject.Object != nullptr &&
                       g_SelectedObject.Object == g_PressedObject.RightObject) &&
                      g_CurrentScreen->OnRightMouseButtonDoubleClick();

        if (result)
        {
            g_PressedObject.ClearRight();
        }
    }
}

void GameInput::OnMidMouseButtonDown(const Core::Vec2<i32>& a_pos)
{
    if (IsInputPossible())
    {
        g_CurrentScreen->OnMidMouseButtonDown();
    }
}

void GameInput::OnMidMouseButtonUp(const Core::Vec2<i32>& a_pos)
{
    if (IsInputPossible())
    {
        g_CurrentScreen->OnMidMouseButtonUp();
    }
}

void GameInput::OnMidMouseButtonDoubleClick(const Core::Vec2<i32>& a_pos)
{
    if (IsInputPossible())
    {
        g_CurrentScreen->OnMidMouseButtonDoubleClick();
    }
}

void GameInput::OnMidMouseButtonScroll(const Core::Vec2<i32>& a_pos, i8 a_delta)
{
    if (IsInputPossible())
    {
        g_CurrentScreen->SelectObject();
        g_CurrentScreen->OnMidMouseButtonScroll(a_delta > 0);
    }
}

void GameInput::OnKeyDown(const Core::KeyEvent& a_event)
{
    if (IsInputPossible())
    {
        g_CurrentScreen->OnKeyDown(a_event);
    }
    // TODO:
    // else if (ch == 0x16 && g_EntryPointer != nullptr)
    // {
    //     if (g_GameState == GS_MAIN)
    //     {
    //         g_MainScreen.Paste();
    //     }
    //     else
    //     {
    //         g_EntryPointer->Paste();
    //     }
    // }
}

void GameInput::OnKeyUp(const Core::KeyEvent& a_event)
{
    if (IsInputPossible())
    {
        g_CurrentScreen->OnKeyUp(a_event);
    }
    // TODO:
    // const auto key = EvKey(ev);
    // if (key == KEY_PRINTSCREEN)
    // {
    //     g_ScreenshotBuilder.SaveScreen();
    // }
}

void GameInput::OnText(const Core::TextEvent& a_event)
{
    if (IsInputPossible())
    {
        g_CurrentScreen->OnTextInput(a_event);
    }
}
