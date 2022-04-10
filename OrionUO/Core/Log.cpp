#include "Log.h"
#include "Core/File.h"
#include "Core/Platform.h"
#include "Core/Time.h"
#include <debugbreak.h>
#include <stdio.h>
#include <time.h>
#if defined(ORION_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#define NOWINMESSAGES
#define NOWINSTYLES
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOKEYSTATES
#define NOSYSCOMMANDS
#define NORASTEROPS
#define NOSHOWWINDOW
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
#define NOGDI
#define NOKERNEL
#define NOUSER
#define NONLS
#define NOMB
#define NOMEMMGR
#define NOMETAFILE
#define NOMINMAX
#define NOMSG
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOWINOFFSETS
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#include <windows.h>
#endif

namespace Core
{

static ILogListener* g_logListeners;
static struct FileLogListener : public ILogListener
{
    FileLogListener() : ILogListener() {}
    ~FileLogListener() { Close(); }

    void Close()
    {
        if (m_file.IsOpen())
        {
            Log::Print(LogVerbosity::Info, "Core", "Closing Log File: %s", m_path.c_str());
            Log::UnregisterLogListener(this);
            m_file.Close();
        }
        m_path.clear();
    }
    void Open(const std::filesystem::path& a_path)
    {
        Close();
        m_path = a_path;
        m_file.Open(a_path, "w");
        if (m_file.IsOpen())
        {
            Log::RegisterLogListener(this);
            Log::Print(LogVerbosity::Info, "Core", "Log File opened: %s", m_path.c_str());
        }
        else
        {
            Log::Print(LogVerbosity::Error, "Core", "Could not open Log File: %s", m_path.c_str());
        }
    }

protected:
    void OnLogMessage(LogVerbosity a_verbosity, const char* a_message) override
    {
        if(m_file.IsOpen())
        {
            m_file.Print(a_message);
            m_file.Flush();
        }
    }

private:
    std::filesystem::path m_path;
    Core::File m_file;
} g_coreFileLogListener;

static struct PrintLogListener : public ILogListener
{
    PrintLogListener() : ILogListener() { Log::RegisterLogListener(this); }
    ~PrintLogListener() { Log::UnregisterLogListener(this); }
    
protected:
    void OnLogMessage(LogVerbosity a_verbosity, const char* a_message) override
    {
        FILE* stream = a_verbosity >= LogVerbosity::Error ? stderr : stdout;
        fprintf(stream, "%s", a_message);
        fflush(stream);
    #if defined(ORION_WINDOWS)
        OutputDebugStringA(a_message);
    #endif // defined(ORION_WINDOWS)
    }

} g_corePrintLogListener;

const char* GetVerbosityName(LogVerbosity a_verbosity)
{
    switch(a_verbosity)
    {
    case LogVerbosity::Debug: return "Debug";
    case LogVerbosity::Info: return "Info";
    case LogVerbosity::Warning: return "Warning";
    case LogVerbosity::Error: return "Error";
    case LogVerbosity::Fatal: return "Fatal";
    default: return "";
    }
}

void Log::Init(const std::filesystem::path& a_path)
{
    g_coreFileLogListener.Open(a_path);
}

void Log::Shutdown()
{
    g_coreFileLogListener.Close();
}

void Log::Print(enum LogVerbosity a_verbosity, const char* a_category, const char *a_format, ...)
{
    enum { LOG_BUFFER_SIZE = 1024 };
    static char logBuffer[LOG_BUFFER_SIZE];

    time_t t = time(NULL);
#if defined(ORION_WINDOWS)
    struct tm tm;
    localtime_s(&tm, &t);
#else
    struct tm tm = *localtime(&t);
#endif
    const int timeLength = (int)strftime(
        logBuffer,
        LOG_BUFFER_SIZE,
        "%Y-%m-%d %H:%M:%S",
        &tm);

    const int prefixLength = snprintf(
        logBuffer + timeLength,
        LOG_BUFFER_SIZE - timeLength,
        "[%09.3fs][%s][%s]",
        Time::GetTotalSeconds(),
        GetVerbosityName(a_verbosity),
        a_category);

    va_list args;
    va_start(args, a_format);    
    const int messageLength = vsnprintf(
        logBuffer + timeLength + prefixLength,
        LOG_BUFFER_SIZE - timeLength - prefixLength,
        a_format,
        args);
    va_end(args);

    int totalLength = timeLength + prefixLength + messageLength;
    if (totalLength >= (LOG_BUFFER_SIZE - 2))
        totalLength = LOG_BUFFER_SIZE - 2;
    logBuffer[totalLength] = '\n';
    logBuffer[totalLength+1] = '\0';

    const char* callstack = a_verbosity == LogVerbosity::Fatal ? Platform::GetCallStack() : nullptr;
    ILogListener* cur = g_logListeners;
    while (cur)
    {
        cur->OnLogMessage(a_verbosity, logBuffer);
        if (callstack)
            cur->OnLogMessage(a_verbosity, callstack);
        cur = cur->m_nextLogListener;
    }
}

void Log::RegisterLogListener(ILogListener* a_listener)
{
    a_listener->m_nextLogListener = g_logListeners;
    g_logListeners = a_listener;
}

void Log::UnregisterLogListener(ILogListener* a_listener)
{
    if (g_logListeners == a_listener)
    {
        g_logListeners = a_listener->m_nextLogListener;
        a_listener->m_nextLogListener = nullptr;
    }
    else
    {
        ILogListener* cur = g_logListeners;
        while (cur && cur->m_nextLogListener != a_listener)
        {
            cur = cur->m_nextLogListener;
        }
        if (cur->m_nextLogListener == a_listener)
        {
            cur->m_nextLogListener = a_listener->m_nextLogListener;
            a_listener->m_nextLogListener = nullptr;
        }
    }
    FATAL_IF_NOT(a_listener->m_nextLogListener == nullptr, "Core", "Failed to unregister Log Listener!");
}


} // namespace Core
