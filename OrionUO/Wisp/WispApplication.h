#pragma once

#include <filesystem>

namespace Wisp
{
class CApplication
{
public:
    CApplication();
    virtual ~CApplication();

    virtual void Init();

    int Run();

    const std::filesystem::path& GetExeDir() const { return m_exeDir; }
    const std::filesystem::path& GetGameDir() const { return m_gameDir; }
    void SetGameDir(const std::filesystem::path& a_path) { m_gameDir = a_path; }

protected:
    virtual void OnMainLoop() {}

protected:
    std::filesystem::path m_exeDir;
    std::filesystem::path m_gameDir;
};

}; // namespace Wisp
