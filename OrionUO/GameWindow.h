#pragma once

#include "Core/Minimal.h"
#include "Core/Window.h"

struct GameWindow : public Core::Window
{
    GameWindow();
    ~GameWindow();

    // Core::Window
    void OnCreated() override;
    void OnDestroy() override;
    void OnResized() override;
    void OnDragging() override;
    void OnActivation(bool a_isActive) override;
    // ~Core::Window
};
extern GameWindow g_gameWindow;
