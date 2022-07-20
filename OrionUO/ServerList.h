#pragma once

#include "Core/DataStream.h"
#include <string>

class CServer
{
public:
    u16 Index        = 0;
    std::string Name = "";
    u8 FullPercent   = 0;
    u8 Timezone      = 0;
    u32 IP           = 0;
    bool Selected    = false;
    int Ping         = -1;
    int PacketsLoss  = -1;

    CServer();
    CServer(u16 index, const std::string& name, u8 fullPercent, u8 timezone, int ip, bool selected);
    ~CServer();
};

class CServerList
{
public:
    std::string LastServerName = "";
    u32 LastServerIndex        = 0;

private:
    std::vector<CServer> m_Servers;

public:
    CServerList();
    ~CServerList();

    CServer* GetServer(int index);
    CServer* GetSelectedServer();
    int ServersCount() { return (int)m_Servers.size(); }
    CServer* Select(int index);
    void ParsePacket(Core::StreamReader& a_reader);
};

extern CServerList g_ServerList;
