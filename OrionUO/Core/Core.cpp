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
    Time::Init();
    Log::Init("game.log");
    Console::Init();
    Input::Init();
}

void BeginFrame()
{
    Time::BeginFrame();
}

void EndFrame()
{
    Time::EndFrame();
}

void Shutdown()
{
    Core::Input::Shutdown();
    Console::Shutdown();
    Log::Shutdown();
    Time::Shutdown();
}

} // namespace Core