#pragma once

#include "Core/Minimal.h"
#include "Core/Window.h"

struct GameWindow : public Core::Window
{
    GameWindow();
    ~GameWindow();

    // Core::Window
    bool OnCreate() override;
    void OnDestroy() override;
    void OnResized() override;
    void OnDragging() override;
    void OnActivation(bool a_isActive) override;
    // ~Core::Window
};
extern GameWindow g_gameWindow;
