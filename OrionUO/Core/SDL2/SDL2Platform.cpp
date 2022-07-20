#include "SDL2Platform.h"
#include <SDL.h>

namespace Core
{

void* Platform::LoadOject(const std::filesystem::path& a_path) { return SDL_LoadObject(a_path.string().c_str()); }
void* Platform::LoadFunction(void* a_object, const char* a_name) { return SDL_LoadFunction(a_object, a_name); }
void Platform::UnloadObject(void* a_object) { SDL_UnloadObject(a_object); }
bool Platform::SetClipboardText(const char* a_text) { return SDL_SetClipboardText(a_text) == 0; }
bool Platform::HasClipboardText() { return SDL_HasClipboardText(); }
const char* Platform::GetClipboardText() { return SDL_GetClipboardText(); }
Core::Rect<int> Platform::GetDisplayArea(int a_display)
{
    SDL_Rect r;
    SDL_GetDisplayUsableBounds(a_display, &r);
    Core::Rect<int> result;
    result.pos.x = r.x;
    result.pos.y = r.y;
    result.size.x = r.w;
    result.size.y = r.h;
    return result;
}

} // namespace Core
