#pragma once

#include "Core/Minimal.h"
#include "Core/Window.h"

struct GameWindow : public Core::Window
{
    GameWindow();
    ~GameWindow();

    // Core::Windowvirtual void OnCreated();
    void OnDestroy() override;
    void OnResized() override;
    void OnActivation(bool a_isActive) override;
    void OnVisibility(bool a_isVisible) override;
    // ~Core::Window
};
extern GameWindow g_gameWindow;
