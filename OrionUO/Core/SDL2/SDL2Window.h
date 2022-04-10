#pragma once

#include "Core/Window.h"

union SDL_Event;

namespace Core
{

namespace SDL2Window
{

void HandleEvent(union SDL_Event* a_event);

};

} // namespace Core
