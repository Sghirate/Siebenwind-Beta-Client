#pragma once

namespace Core
{

struct SDL2App
{
    static bool Init();
    static void BeginFrame();
    static void EndFrame();
    static void Shutdown();
};

} // namespace Core
