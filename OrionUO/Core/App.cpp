#include "App.h"
#include "Core/CommandLine.h"
#include "Core/Core.h"
#include "Core/Log.h"
#include "Core/Platform.h"
#if defined(ORION_WITH_SDL2)
#include "Core/SDL2/SDL2App.h"
#endif // defined(ORION_WITH_SDL2)
#include <cstring>

namespace Core
{

App::App()
{
}

App::~App()
{
}

int App::Run()
{
    while (!IsTerminating())
    {
        Core::BeginFrame();
        BeginFrame();
        TickFrame();
        EndFrame();
        Core::EndFrame();
    }
    return EXIT_SUCCESS;
}

bool App::Init()
{
#if defined(ORION_WITH_SDL2)
    if (!SDL2App::Init())
        return false;
#endif // defined(ORION_WITH_SDL2)
    LOG_INFO("Core", "Initialize Application");
    m_exeDir = Platform::GetBinaryPath().parent_path();
    LOG_INFO("Core", "ExeDir: %s", m_exeDir.string().c_str());

    m_gameDir = std::filesystem::current_path();
    for (int i = 1; i < CommandLine::GetArgc(); ++i)
    {
        char* arg = CommandLine::GetArgv()[i];
        if (((strcmp(arg, "--game") == 0) || strcmp(arg, "-g") == 0) && i < CommandLine::GetArgc())
        {
            m_gameDir = std::filesystem::path(CommandLine::GetArgv()[i+1]);
            ++i;
        }
    }
    LOG_INFO("Core", "GameDir: %s", m_gameDir.string().c_str());
    return true;
}

void App::BeginFrame()
{
#if defined(ORION_WITH_SDL2)
    SDL2App::BeginFrame();
#endif // defined(ORION_WITH_SDL2)
}

void App::TickFrame()
{
}

void App::EndFrame()
{
#if defined(ORION_WITH_SDL2)
    SDL2App::EndFrame();
#endif // defined(ORION_WITH_SDL2)
}

void App::Shutdown()
{
#if defined(ORION_WITH_SDL2)
    SDL2App::Shutdown();
#endif // defined(ORION_WITH_SDL2)
}


} // namespace Core
