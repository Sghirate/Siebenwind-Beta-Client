#include "SDL2Util.h"
#include <SDL.h>

namespace Core
{

bool SDL2Util::IsEventCategory(SDL_Event* a_event, SDL_EventCategory a_category)
{
    return (a_event->type & a_category) == a_category;
}

} // namespace Core
