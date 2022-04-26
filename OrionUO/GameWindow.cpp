#include "GameWindow.h"
#include "Core/Log.h"
#include "Core/Window.h"
// TODO: REMOVE
#include "GameInput.h"
#include "ClickObject.h"
#include "GLEngine/GLEngine.h"
#include "Globals.h"
#include "Managers/ConfigManager.h"
#include "Managers/MouseManager.h"
#include "Managers/ScreenEffectManager.h"
#include "Managers/SoundManager.h"
#include "OrionUO.h"
#include "PressedObject.h"
#include "ScreenStages/BaseScreen.h"
#include "ScreenStages/MainScreen.h"
#include "ScreenStages/ServerScreen.h"
#include "SelectedObject.h"
// ~TODO: REMOVE

GameWindow g_gameWindow;

GameWindow::GameWindow()
{
}

GameWindow::~GameWindow()
{
}

void GameWindow::OnCreated()
{
    Core::Window::OnCreated();
    // TODO: REMOVE
    if (!g_GL.Install())
    {
        LOG_ERROR("GameWindow", "Could not install OpenGL!");
        return;
    }
    if (!g_Orion.Install())
    {
        return;
    }
    g_GL.UpdateRect();
    GameInput::Get().Init();
    // ~TODO: REMOVE
}

void GameWindow::OnDestroy()
{
    Core::Window::OnDestroy();
    // TODO: REMOVE
    GameInput::Get().Shutdown();
    g_SoundManager.Free();
    g_Orion.Uninstall();
    // ~TODO: REMOVE
}

void GameWindow::OnResized()
{
    Core::Window::OnResized();
    // TODO: REMOVE
    g_GL.UpdateRect();
    // ~TODO: REMOVE
}

void GameWindow::OnDragging()
{
    Core::Window::OnDragging();
    if (g_CurrentScreen != nullptr && g_ScreenEffectManager.Mode == SEM_NONE)
    {
        g_CurrentScreen->OnDragging();
    }
}

void GameWindow::OnActivation(bool a_isActive)
{
    Core::Window::OnActivation(a_isActive);
    // TODO: REMOVE
    if (a_isActive)
        g_SoundManager.ResumeSound();
    else if (!g_ConfigManager.BackgroundSound)
        g_SoundManager.PauseSound();
    // ~TODO: REMOVE
}
