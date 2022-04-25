#include "Main.h"
#include "Core/Core.h"
#include "OrionApplication.h"

int Main(int argc, char** argv)
{
    Core::ScopedCore core(argc, argv);
    g_app.Init(argc, argv);
    g_ConfigManager.Init();

    if (!g_sotWindow.Create("SoT", "SoT", true, 640, 480))
    {
        LOG_F(ERROR, "Could not create game window!");
        return -1;
    }

    int result = g_app.Run();
    SDL_Quit();
    return result;
}

#if (_WIN32)

struct WinArgs
{
    WinArgs();
    ~WinArgs();

    bool IsValid() const { return m_argv != nullptr; }
    int& GetArgc() { return m_argc; }
    char** GetArgv() { return m_argv; }

private:
    int m_argc;
    char** m_argv = nullptr;
};
WinArgs::WinArgs()
    : m_argc(__argc)
{
    if (__argv)
    {
        m_argv = (char**)malloc(sizeof(char*) * (m_argc + 1));
        memset(m_argv, 0, sizeof(char*) * (m_argc + 1));
        for (int i = 0; i < m_argc; ++i)
        {
            size_t n = strlen(__argv[i]);
            m_argv[i] = (char*)malloc(sizeof(char) * n + 1);
            strncpy_s(m_argv[i], n + 1, __argv[i], n);
        }
    }
    else if (__wargv)
    {
        m_argv = (char**)malloc(sizeof(char*) * (m_argc + 1));
        memset(m_argv, 0, sizeof(char*) * (m_argc + 1));
        for (int i = 0; i < m_argc; ++i)
        {
            int size = WideCharToMultiByte(CP_UTF8, 0, __wargv[i], -1, NULL, 0, NULL, NULL);
            m_argv[i] = (char*)malloc(sizeof(char) * size);
            WideCharToMultiByte(CP_UTF8, 0, __wargv[i], -1, m_argv[i], size, NULL, NULL);
        }
    }
}

WinArgs::~WinArgs()
{
    if (m_argv)
    {
        for (int i = 0; i < m_argc; ++i)
            free(m_argv[i]);
        free(m_argv);
    }
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    WinArgs args;
    if (!args.IsValid())
        return -1;

    return Main(args.GetArgc(), args.GetArgv());
}

#else

int main(int argc, char* argv[])
{
    return Main(argc, argv);
}

#endif