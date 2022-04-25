#include <SDL_timer.h>
#include "OrionApplication.h"
#include "OrionUO.h"
#include "Managers/ConnectionManager.h"
#include "Managers/PacketManager.h"

COrionApplication g_App;

void COrionApplication::TickFrame()
{
    Core::App::TickFrame();

    g_Ticks = SDL_GetTicks();

    g_ConnectionManager.Recv();
    g_PacketManager.ProcessPluginPackets();
    g_PacketManager.SendMegaClilocRequests();
    g_Orion.Process(true);
}
