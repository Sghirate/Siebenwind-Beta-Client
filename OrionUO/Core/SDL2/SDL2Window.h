#pragma once

#include "Core/Window.h"

union SDL_Event;

namespace Core
{

namespace SDL2Window
{

u32 GetWindowID(Window* a_window);
Window* GetWindow(u32 a_id);
void HandleEvent(union SDL_Event* a_event);

};

} // namespace Core
