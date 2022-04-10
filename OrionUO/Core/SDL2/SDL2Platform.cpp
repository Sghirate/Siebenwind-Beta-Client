#include "SDL2Platform.h"
#include <SDL.h>

namespace Core
{

void* Platform::LoadOject(const std::filesystem::path& a_path) { return SDL_LoadObject(a_path.string().c_str()); }
void* Platform::LoadFunction(void* a_object, const char* a_name) { return SDL_LoadFunction(a_object, a_name); }
void Platform::UnloadObject(void* a_object) { SDL_UnloadObject(a_object); }

} // namespace Core
