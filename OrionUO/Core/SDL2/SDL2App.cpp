#include "SDL2App.h"
#include "Core/SDL2/SDL2Input.h"
#include "Core/SDL2/SDL2Window.h"
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
    SDL_Event e;
    while(SDL_PollEvent(&e))
    {
        SDL2Input::Get().HandleEvent(&e);
        SDL2Window::HandleEvent(&e);
    }
}

void SDL2App::EndFrame()
{

}

void SDL2App::Shutdown()
{
    SDL_Quit();
}

} // namespace Core
