#include "PingThread.h"
#include "Core/Log.h"
#include "Sockets.h"
#include <SDL_events.h>
#include <SDL_timer.h>
#include <atomic>


static std::atomic<i32> s_pingCnt;

CPingThread::CPingThread(int serverID, const std::string &serverIP, int requestsCount)
    : ServerID(serverID)
    , ServerIP(serverIP)
    , RequestsCount(requestsCount)
{
    //assert(s_pingCnt == 0 && "Multiple ping threads running at the same time");
    s_pingCnt++;
    LOG_INFO("PingThread", "Initialize: %s", serverIP.c_str());
}

CPingThread::~CPingThread()
{
    //assert(s_pingCnt == 1 && "Multiple ping threads running at the same time");
    s_pingCnt--;
}

int CPingThread::CalculatePing()
{
    auto handle = icmp_open();
    if (handle == nullptr)
    {
        return -4;
    }

    u32 start = SDL_GetTicks();
    int result = icmp_query(handle, ServerIP.c_str(), &start);
    if (result == 0)
    {
        result = (SDL_GetTicks() - start);
    }

    icmp_close(handle);
    return result;
}

void CPingThread::OnExecute(u32 nowTime)
{

    if (ServerIP.empty() || RequestsCount < 1)
    {
        return;
    }

    auto *info = new PING_INFO_DATA;
    info->ServerID = ServerID;
    for (int i = 0; i < RequestsCount; i++)
    {
        const int ping = CalculatePing();
        if (ping < 0)
        {
            if (ping == -1)
            {
                info->Lost++;
            }
            continue;
        }

        info->Min = Core::Min(info->Min, ping);
        info->Max = Core::Max(info->Max, ping);
        info->Average += (info->Max - info->Min);
    }

    info->Average = info->Min + (info->Average / RequestsCount);
    PUSH_EVENT(MessageID, info, nullptr);
}
