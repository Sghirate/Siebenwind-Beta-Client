#include "Vars.h"

namespace
{
struct VerbosityMapping
{
    LogVerbosity verbosity;
    std::string string;
} g_verbosityMappings[] = {
    { LogVerbosity::Debug,   "debug"   },
    { LogVerbosity::Info,    "info"    },
    { LogVerbosity::Warning, "warning" },
    { LogVerbosity::Error,   "error"   },
    { LogVerbosity::Fatal,   "fatal"   },
};
static bool LogVerbosityFromString(const std::string& a_string, LogVerbosity& out_verbosity)
{
    const int n = sizeof(g_verbosityMappings)/sizeof(g_verbosityMappings[0]);
    for (int i = 0; i < n; ++i)
    {
        const VerbosityMapping& mapping = g_verbosityMappings[i];
        if (a_string.length() != mapping.string.length())
        {
            continue;
        }
        bool match = true;
        for (int i = 0; i < a_string.length(); ++i)
        {
            char a = a_string[i];
            if (a >= 'A' && a <= 'Z')
            {
                a = 'a' + (a - 'A');
            }
            char b = mapping.string[i];
            if (b >= 'A' && b <= 'Z')
            {
                b = 'a' + (b - 'A');
            }
            match &= a == b;
            if (!match)
            {
                break;
            }
        }
        if (match)
        {
            out_verbosity = mapping.verbosity;
            return true;
        }
    }
    return false;
}
LogVerbosity g_consoleLogVerbosity = LogVerbosity::Warning;
static void OnConsoleLogVerbosityChanged()
{
    if (!LogVerbosityFromString(core_console_verbosity.GetValue(), g_consoleLogVerbosity))
    {
        LOG_ERROR("Core", "Could not parse console log verbosity: %s", core_console_verbosity.GetValue().c_str());
    }
}
LogVerbosity g_fileLogVerbosity = LogVerbosity::Warning;
static void OnFileLogVerbosityChanged()
{
    if (!LogVerbosityFromString(core_file_verbosity.GetValue(), g_fileLogVerbosity))
    {
        LOG_ERROR("Core", "Could not parse file log verbosity: %s", core_file_verbosity.GetValue().c_str());
    }
}

#ifdef DEBUG
static const char* kDefaultConsoleLogVerbosity = "info";
static const char* kDefaultFileLogVerbosity = "info";
#else
static const char* kDefaultConsoleLogVerbosity = "warning";
static const char* kDefaultFileLogVerbosity = "warning";
#endif

} // <anonymous> namespace

namespace Core
{

LogVerbosity Vars::GetConsoleLogVerbosity()
{
    return g_consoleLogVerbosity;
}

LogVerbosity Vars::GetFileLogVerbosity()
{
    return g_fileLogVerbosity;
}

}

Core::ConsoleVariable<std::string> core_console_verbosity("core_console_verbosity", kDefaultConsoleLogVerbosity, Core::ConsoleFlags::User, OnConsoleLogVerbosityChanged);
Core::ConsoleVariable<std::string> core_file_verbosity("core_file_verbosity", kDefaultFileLogVerbosity, Core::ConsoleFlags::User, OnFileLogVerbosityChanged);