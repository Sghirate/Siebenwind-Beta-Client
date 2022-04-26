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

struct GameInputHandler : public Core::IMouseListener
{
    void Register() { Core::Input::RegisterMouseListener(this); }
    void Unregister() { Core::Input::UnregisterMouseListener(this); }

    void OnMouseButton(
        u8 a_mouseIndex,
        Core::Window* a_focusWindow,
        const Core::TMousePos& a_pos,
        Core::EMouseButton a_button,
        bool a_state,
        u8 a_clicks) override
    {
        if (a_focusWindow == &g_gameWindow)
        {
            switch (a_button)
            {
                case Core::EMouseButton::Button_Left:
                    if (a_state)
                    {
                        if (a_clicks > 1)
                            GameInput::Get().OnLeftMouseButtonDoubleClick(a_pos);
                        else
                            GameInput::Get().OnLeftMouseButtonDown(a_pos);
                    }
                    else
                    {
                        GameInput::Get().OnLeftMouseButtonUp(a_pos);
                    }
                    break;
                case Core::EMouseButton::Button_Middle:
                    if (a_state)
                        GameInput::Get().OnMidMouseButtonDown(a_pos);
                    else
                        GameInput::Get().OnMidMouseButtonUp(a_pos);
                    break;
                case Core::EMouseButton::Button_Right:
                    if (a_state)
                        GameInput::Get().OnRightMouseButtonDown(a_pos);
                    else
                        GameInput::Get().OnRightMouseButtonUp(a_pos);
                    break;
                default: break;
            }
        }
    }

    void OnMouseWheel(
        u8 a_mouseIndex,
        Core::Window* a_focusWindow,
        const Core::TMousePos& a_pos,
        const Core::TMouseWheelDelta& a_delta) override
    {
        if (a_focusWindow == &g_gameWindow)
        {
            GameInput::Get().OnMidMouseButtonScroll(a_pos, a_delta.y);
        }
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
