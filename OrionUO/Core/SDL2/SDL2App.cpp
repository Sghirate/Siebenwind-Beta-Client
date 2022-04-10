#include "SDL2App.h"
#include "Core/Log.h"
#include <SDL.h>

namespace Core
{

bool SDL2App::Init()
{
    if (SDL_Init(SDL_INIT_TIMER) < 0)
    {
        LOG_ERROR("Core::SDL2", "Could not initialize SDL: %s", SDL_GetError());
        return false;
    }
    return true;
}

void SDL2App::BeginFrame()
{

}

void SDL2App::EndFrame()
{

}

void SDL2App::Shutdown()
{
    SDL_Quit();
}

} // namespace Core
