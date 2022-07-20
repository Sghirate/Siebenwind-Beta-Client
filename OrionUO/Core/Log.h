#pragma once

#include <filesystem>
#include <stdarg.h>

enum class LogVerbosity
{
    Debug,
    Info,
    Warning,
    Error,
    Fatal,
};

#define LOG(verbosity, category, format, ...) Core::Log::Print(verbosity, category, format, ##__VA_ARGS__)
#define LOG_DEBUG(category, format, ...) LOG(LogVerbosity::Debug, category, format, ##__VA_ARGS__)
#define LOG_INFO(category, format, ...) LOG(LogVerbosity::Info, category, format, ##__VA_ARGS__)
#define LOG_WARNING(category, format, ...) LOG(LogVerbosity::Warning, category, format, ##__VA_ARGS__)
#define LOG_ERROR(category, format, ...) LOG(LogVerbosity::Error, category, format, ##__VA_ARGS__)
#define LOG_FATAL(category, format, ...) LOG(LogVerbosity::Fatal, category, format, ##__VA_ARGS__)

#define ERROR_IF(condition, category, format, ...) if ((condition)) { LOG(LogVerbosity::Error, category, format, ##__VA_ARGS__); }
#define ERROR_IF_NOT(condition, category, format, ...) if(!(condition)) { LOG(LogVerbosity::Error, category, format, ##__VA_ARGS__); }
#define FATAL_IF(condition, category, format, ...) if ((condition)) { LOG(LogVerbosity::Fatal, category, format, ##__VA_ARGS__); }
#define FATAL_IF_NOT(condition, category, format, ...) if(!(condition)) { LOG(LogVerbosity::Fatal, category, format, ##__VA_ARGS__); }

namespace Core
{

struct ILogListener;
struct Log
{
    static void Init(const class std::filesystem::path& a_path);
    static void Shutdown();
    static void Print(enum LogVerbosity a_verbosity, const char* a_category, const char *a_format, ...);
    static void RegisterLogListener(ILogListener* a_listener);
    static void UnregisterLogListener(ILogListener* a_listener);
};
struct ILogListener
{
    virtual ~ILogListener() {}

protected:
    ILogListener() {}
    virtual void OnLogMessage(LogVerbosity a_verbosity, const char* a_message) = 0;

private:
    friend struct Log;
    ILogListener* m_nextLogListener = nullptr;
};

} // namespace Core
