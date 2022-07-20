#include "ServerList.h"
#include "Core/Log.h"
#include "OrionUO.h"
#include "ScreenStages/MainScreen.h"
#include "ScreenStages/ServerScreen.h"
#include "Utility/Pinger.h"

namespace
{

static void OnPingResult(const PingResult& a_result)
{
    if (a_result.min >= 9999)
        return;

    if (CServer* server = g_ServerList.GetServer(a_result.serverId))
    {
        server->Ping = a_result.avg;
        server->PacketsLoss = a_result.lost;
        g_ServerScreen.UpdateContent();
    }
}
static void PingServer(u32 a_id, u32 a_ip)
{
    if (ping_disable.GetValue() != 0)
        return;
    
    char ipString[30] = { 0 };
    sprintf_s(
        ipString,
        "%i.%i.%i.%i",
        (a_ip >> 24) & 0xFF,
        (a_ip >> 16) & 0xFF,
        (a_ip >> 8) & 0xFF,
        a_ip & 0xFF);
    Pinger::Get().Ping(a_id, ipString, 100, OnPingResult);
}

} // namespace <anonymous>

CServerList g_ServerList;

CServer::CServer()
{
}

CServer::CServer(
    u16 index, const std::string& name, u8 fullPercent, u8 timezone, int ip, bool selected)
    : Index(index)
    , Name(name)
    , FullPercent(fullPercent)
    , Timezone(timezone)
    , IP(ip)
    , Selected(selected)
{
}

CServer::~CServer()
{
}

CServerList::CServerList()
{
}

CServerList::~CServerList()
{
    m_Servers.clear();
}

void CServerList::ParsePacket(Core::StreamReader& a_reader)
{
    m_Servers.clear();
    g_ServerList.LastServerIndex = 0;

    a_reader.Move(1);
    u16 numServers = a_reader.ReadBE<u16>();

    if (numServers == 0)
    {
        LOG_WARNING("ServerList", "Empty server list!");
    }

    for (u16 i = 0; i < numServers; i++)
    {
        u16 id           = a_reader.ReadBE<u16>();
        std::string name = a_reader.ReadString(32);
        u8 fullPercent   = a_reader.ReadLE<u8>();
        u8 timezone      = a_reader.ReadLE<u8>();
        u32 ip           = a_reader.ReadLE<u32>(); //little-endian!!!

        const bool selected = (name == g_ServerList.LastServerName);
        if (selected)
        {
            g_ServerList.LastServerIndex = (int)i;
        }
        m_Servers.emplace_back(id, name, fullPercent, timezone, ip, selected);
        PingServer(i, ip);
    }

    if (g_ServerList.LastServerIndex < numServers && g_MainScreen.m_AutoLogin->Checked)
    {
        g_Orion.ServerSelection(g_ServerList.LastServerIndex);
    }
    else
    {
        g_Orion.InitScreen(GS_SERVER);
    }

    g_ServerScreen.UpdateContent();
}

CServer* CServerList::GetServer(int index)
{
    if (index < (int)m_Servers.size())
    {
        return &m_Servers[index];
    }

    return nullptr;
}

CServer* CServerList::GetSelectedServer()
{
    for (CServer& server : m_Servers)
    {
        if (server.Selected)
        {
            return &server;
        }
    }

    return nullptr;
}

CServer* CServerList::Select(int index)
{
    CServer* server = nullptr;

    for (int i = int(m_Servers.size()) - 1; i >= 0; i--)
    {
        if (index == i)
        {
            server                = &m_Servers[i];
            m_Servers[i].Selected = true;
        }
        else
        {
            m_Servers[i].Selected = false;
        }
    }

    return server;
}
