#pragma once

#include "Core/Minimal.h"
#include "Core/Input.h"

struct GameInput
{
    static GameInput& Get();

    void Init();
    void Shutdown();

    bool IsInputPossible() const;

    void OnMouseMove(const Core::Vec2<i32>& pos);
    void OnLeftMouseButtonDown(const Core::Vec2<i32>& a_pos);
    void OnLeftMouseButtonUp(const Core::Vec2<i32>& a_pos);
    void OnLeftMouseButtonDoubleClick(const Core::Vec2<i32>& a_pos);
    void OnRightMouseButtonDown(const Core::Vec2<i32>& a_pos);
    void OnRightMouseButtonUp(const Core::Vec2<i32>& a_pos);
    void OnRightMouseButtonDoubleClick(const Core::Vec2<i32>& a_pos);
    void OnMidMouseButtonDown(const Core::Vec2<i32>& a_pos);
    void OnMidMouseButtonUp(const Core::Vec2<i32>& a_pos);
    void OnMidMouseButtonDoubleClick(const Core::Vec2<i32>& a_pos);
    void OnMidMouseButtonScroll(const Core::Vec2<i32>& a_pos, i8 a_delta);
    void OnKeyDown(const Core::KeyEvent& a_event);
    void OnKeyUp(const Core::KeyEvent& a_event);
    void OnText(const Core::TextEvent& a_event);

private:
    GameInput();
    ~GameInput();
};
