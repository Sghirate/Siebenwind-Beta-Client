#pragma once

namespace Core
{

void Init(int a_argc, char** a_argv);
void BeginFrame();
void EndFrame();
void Shutdown();

struct ScopedCore
{
    ScopedCore(int a_argc, char** a_argv) { Init(a_argc, a_argv); }
    ~ScopedCore() { Shutdown(); }
};

} // namespace Core
