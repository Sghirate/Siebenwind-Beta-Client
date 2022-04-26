#include "Pinger.h"

Core::ConsoleVariable<u8> ping_disable("ping_disable", 0, Core::ConsoleFlags::Persistent);

Pinger& Pinger::Get()
{
    static Pinger s_instance;
    return s_instance;
}

void Pinger::Ping(u32 a_serverId, const std::string& a_serverAddress, int a_count, PingCallback a_callback)
{

}

Pinger::Pinger()
{
}

Pinger::~Pinger()
{
}

