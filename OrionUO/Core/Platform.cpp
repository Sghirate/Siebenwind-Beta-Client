#include "Platform.h"
#define B_STACKTRACE_IMPL
#include "b_stacktrace.h"
#include <vector>
#include <whereami.c>

namespace Core
{

const char* Platform::GetCallStack()
{
    return b_stacktrace_get();
}

std::filesystem::path Platform::GetBinaryPath()
{
    std::vector<char> buffer;
    int size = wai_getExecutablePath(nullptr, 0, nullptr);
    buffer.resize(size+1);
    wai_getExecutablePath(buffer.data(), size, nullptr);
    buffer[size] = '\0';
    return std::filesystem::path(&buffer[0]);
}

std::filesystem::path Platform::GetModulePath()
{
    std::vector<char> buffer;
    int size = wai_getModulePath(nullptr, 0, nullptr);
    buffer.resize(size+1);
    wai_getModulePath(buffer.data(), size, nullptr);
    buffer[size] = '\0';
    return std::filesystem::path(&buffer[0]);
}

} // namespace Core
