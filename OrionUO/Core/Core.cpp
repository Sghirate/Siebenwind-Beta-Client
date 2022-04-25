#include "Core/Core.h"
#include "Core/CommandLine.h"
#include "Core/Console.h"
#include "Core/Input.h"
#include "Core/Log.h"
#include "Core/Time.h"

namespace Core
{

void Init(int a_argc, char** a_argv)
{
    CommandLine::Set(a_argc, a_argv);
    Log::Init("game.log");
    Console::Init();
    Input::Init();
}

void BeginFrame()
{
    FrameTimer::Get().BeginFrame();
}

void EndFrame()
{
    FrameTimer::Get().EndFrame();
}

void Shutdown()
{
    Core::Input::Shutdown();
    Console::Shutdown();
    Log::Shutdown();
}

} // namespace Core