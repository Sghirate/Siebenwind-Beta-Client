#pragma once

#include "Core/Console.h"
#include "Core/Log.h"
#include <string>

namespace Core
{

struct Vars
{
static LogVerbosity GetConsoleLogVerbosity();
static LogVerbosity GetFileLogVerbosity();
};

}

extern Core::ConsoleVariable<std::string> core_console_verbosity;
extern Core::ConsoleVariable<std::string> core_file_verbosity;
extern Core::ConsoleVariable<int> core_no_sound;