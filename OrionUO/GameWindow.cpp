#include "GameWindow.h"
#include "Core/Window.h"

GameWindow::GameWindow()
{
}

GameWindow::~GameWindow()
{
}

void GameWindow::OnDestroy()
{
    Core::Window::OnDestroy();
}

void GameWindow::OnResized()
{
    Core::Window::OnResized();
}

void GameWindow::OnActivation(bool a_isActive)
{
    Core::Window::OnActivation(a_isActive);
}

void GameWindow::OnVisibility(bool a_isVisible)
{
    Core::Window::OnVisibility(a_isVisible);
}
