#pragma once

namespace Core
{

void Init(int a_argc, char** a_argv);
void BeginFrame();
void EndFrame();
void Shutdown();

struct ScopedCore
{
    ScopedCore(int a_argc, char** a_argv) { Core::Init(a_argc, a_argv); }
    void BeginFrame() { Core::BeginFrame(); }
    void EndFrame() { Core::EndFrame(); }
    ~ScopedCore() { Core::Shutdown(); }
};

} // namespace Core
