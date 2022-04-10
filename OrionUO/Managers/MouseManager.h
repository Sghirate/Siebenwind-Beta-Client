#pragma once

#include "Core/Input.h"

class MouseManager
{
private:
    int m_CursorOffset[2][16];

public:
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
};

extern MouseManager g_MouseManager;
