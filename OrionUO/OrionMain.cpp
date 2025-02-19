#include "Config.h"
#include "GameWindow.h"
#include "OrionUO.h"
#include "OrionApplication.h"
#include <SDL.h>
#include <time.h>
#include "Managers/ConfigManager.h"
#include "Core/Core.h"
#include "Core/Log.h"
#include "SiebenwindClient.h"

static bool g_isHeadless = false;

int main(int argc, char **argv)
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
