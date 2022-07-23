#include "Main.h"
#include "Core/Core.h"
#include "Core/Log.h"
#include "Core/Window.h"
#include "Config.h"
#include "GameWindow.h"
#include "Managers/ConfigManager.h"
#include "OrionApplication.h"
#include "SiebenwindClient.h"
#include "plugin/enumlist.h"
#include <cstring>

static bool g_isHeadless = false;

int Main(int argc, char** argv)
{
    Core::ScopedCore core(argc, argv);
    if (!g_App.Init())
        return EXIT_FAILURE;
    LoadGlobalConfig();

    // TODO: good cli parsing api
    // keep this simple for now just for travis-ci
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "--headless") == 0)
        {
            g_isHeadless = true;
        }
        else if (strcmp(argv[i], "--nocrypt") == 0)
        {
            g_Config.EncryptionType = ET_NOCRYPT;
        }
    }

    if (!g_isHeadless)
    {
        Core::TWindowPosition pos;
        Core::TWindowSize size(640, 480);
        if (!g_gameWindow.Create(SiebenwindClient::GetWindowTitle().c_str(), pos, size))
        {
            LOG_WARNING("Game", "Failed to create client window. Fallbacking to headless mode.");
            g_isHeadless = true;
        }
        g_gameWindow.HideCursor();
    }
    g_ConfigManager.Init();

    // FIXME: headless: lets end here so we can run on travis for now
    if (g_isHeadless)
    {
        return EXIT_SUCCESS;
    }

    auto ret = g_App.Run();
    g_App.Shutdown();
    return ret;
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

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
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