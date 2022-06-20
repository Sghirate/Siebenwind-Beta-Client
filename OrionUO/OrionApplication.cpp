#include <SDL_timer.h>
#include "OrionApplication.h"
#include "OrionUO.h"
#include "Globals.h"
#include "Managers/ConnectionManager.h"
#include "Managers/PacketManager.h"
#include "GameWindow.h"
#include "Profiler.h"

COrionApplication g_App;

void COrionApplication::TickFrame()
{
    PROFILER_BEGIN_FRAME();

    Core::App::TickFrame();

    g_Ticks = SDL_GetTicks();

    g_ConnectionManager.Recv();
    g_PacketManager.ProcessPluginPackets();
    g_PacketManager.SendMegaClilocRequests();
    g_Orion.Process(true);

    PROFILER_END_FRAME();
}

bool COrionApplication::IsTerminating() const
{
    return !g_gameWindow.IsOpen();
}
