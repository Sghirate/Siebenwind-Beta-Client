#pragma once

#include "Core/Input.h"

class MouseManager
{
private:
    int m_CursorOffset[2][16];

public:
    // TODO: Remove
    Core::TMousePos LeftDropPosition = Core::Vec2<i32>();
    Core::TMousePos RightDropPosition = Core::Vec2<i32>();
    Core::TMousePos MidDropPosition = Core::Vec2<i32>();
    u32 LastLeftButtonClickTimer = 0;
    u32 LastRightButtonClickTimer = 0;
    u32 LastMidButtonClickTimer = 0;
    bool LeftButtonPressed = false;
    bool RightButtonPressed = false;
    bool MidButtonPressed = false;
    bool Dragging = false;
    int DoubleClickDelay = 350;
    bool CancelDoubleClick = false;
    // ~TODO

    MouseManager() {}
    ~MouseManager() {}

    bool LoadCursorTextures();
    int Sgn(int val);
    int GetFacing(int x1, int y1, int to_x, int to_y, int current_facing);
    u16 GetGameCursor();
    void ProcessWalking();
    void Draw(u16 id);
    Core::TMousePos GetPosition() const { return Core::Input::GetMouse()->GetPosition(); }
    void SetPosition(const Core::TMousePos& a_pos) { Core::Input::GetMouse()->SetPosition(a_pos); }
    Core::TMousePos GetLeftDroppedOffset() const { return GetPosition() - LeftDropPosition; }
    Core::TMousePos GetRightDroppedOffset() const { return GetPosition() - RightDropPosition; }
    Core::TMousePos GetMidDroppedOffset() const { return GetPosition() - MidDropPosition; }

    void EmulateOnLeftMouseButtonDown();
};

extern MouseManager g_MouseManager;
