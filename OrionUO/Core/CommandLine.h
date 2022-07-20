#pragma once

namespace Core
{

struct CommandLine
{
    static void Set(int a_argc, char** a_argv)
    {
        s_argc = a_argc;
        s_argv = a_argv;
    }
    static int GetArgc() { return s_argc; }
    static char** GetArgv() { return s_argv; }

private:
    static int s_argc;
    static char** s_argv;
};

} // namespace Core
