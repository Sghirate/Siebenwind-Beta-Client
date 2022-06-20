#pragma once

#include <filesystem>

namespace Core
{

struct App
{
    App();
    virtual ~App();

    const std::filesystem::path& GetExeDir() const { return m_exeDir; }
    const std::filesystem::path& GetGameDir() const { return m_gameDir; }
    void SetGameDir(const std::filesystem::path& a_path) { m_gameDir = a_path; }
    void* GetHandle() const { return m_handle; }
    void SetHandle(void* a_handle) { m_handle = a_handle; }
    int Run();

    virtual bool Init();
    virtual void BeginFrame();
    virtual void TickFrame();
    virtual void EndFrame();
    virtual void Shutdown();
    virtual bool IsTerminating() const { return false; }

protected:
    std::filesystem::path m_exeDir;
    std::filesystem::path m_gameDir;
    void* m_handle;
};

} // namespace Core
