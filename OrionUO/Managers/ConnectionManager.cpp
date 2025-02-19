#include "ConnectionManager.h"
#include "Core/Log.h"
#include "GameVars.h"
#include "Globals.h"
#include "PacketManager.h"
#include <SDL_stdinc.h>
#include "Config.h"
#include "OrionUO.h"
#include "Profiler.h"
#include "ScreenStages/ConnectionScreen.h"
#include "ScreenStages/GameBlockedScreen.h"
#include "Crypt/CryptEntry.h"

CConnectionManager g_ConnectionManager;

CConnectionManager::CConnectionManager()
{
}

CConnectionManager::~CConnectionManager()
{
    if (m_LoginSocket.IsConnected())
    {
        m_LoginSocket.Disconnect();
    }

    if (m_GameSocket.IsConnected())
    {
        m_GameSocket.Disconnect();
    }
}

void CConnectionManager::SetUseProxy(bool val)
{
    m_UseProxy = val;
    m_LoginSocket.UseProxy = val;
    m_GameSocket.UseProxy = val;
}

void CConnectionManager::SetProxyAddress(const std::string &val)
{
    m_ProxyAddress = val;
    m_LoginSocket.ProxyAddress = val;
    m_GameSocket.ProxyAddress = val;
}

void CConnectionManager::SetProxyPort(int val)
{
    m_ProxyPort = val;
    m_LoginSocket.ProxyPort = val;
    m_GameSocket.ProxyPort = val;
}

void CConnectionManager::SetProxySocks5(bool val)
{
    m_ProxySocks5 = val;
    m_LoginSocket.ProxySocks5 = val;
    m_GameSocket.ProxySocks5 = val;
}

void CConnectionManager::SetProxyAccount(const std::string &val)
{
    m_ProxyAccount = val;
    m_LoginSocket.ProxyAccount = val;
    m_GameSocket.ProxyAccount = val;
}

void CConnectionManager::SetProxyPassword(const std::string &val)
{
    m_ProxyPassword = val;
    m_LoginSocket.ProxyPassword = val;
    m_GameSocket.ProxyPassword = val;
}

void CConnectionManager::Init()
{
    if (m_LoginSocket.IsConnected())
    {
        return;
    }

    m_LoginSocket.Init();
    m_GameSocket.Init();

    m_IsLoginSocket = true;
    const auto localIp = Core::Socket::GetLocalAddress();
    m_Seed[0] = static_cast<unsigned char>((localIp >> 24) & 0xff);
    m_Seed[1] = static_cast<unsigned char>((localIp >> 16) & 0xff);
    m_Seed[2] = static_cast<unsigned char>((localIp >> 8) & 0xff);
    m_Seed[3] = static_cast<unsigned char>(localIp & 0xff);

    Crypt::Init(true, m_Seed);
}

void CConnectionManager::Init(u8 *gameSeed)
{
    if (m_GameSocket.IsConnected())
    {
        return;
    }

    m_IsLoginSocket = false;
    Crypt::Init(false, gameSeed);
}

void CConnectionManager::SendIP(CSocket &socket, u8 *ip)
{
    socket.Send(ip, 4);
}

bool CConnectionManager::Connect(const std::string &address, int port, u8 *gameSeed)
{
    LOG_INFO("ConnectionManager", "Connecting %s:%d", address.c_str(), port);
    if (m_IsLoginSocket)
    {
        if (m_LoginSocket.IsConnected())
        {
            return true;
        }

        bool result = m_LoginSocket.Connect(address, port);
        if (result)
        {
            LOG_INFO("ConnectionManager", "connected");
            g_TotalSendSize = 4;
            g_LastPacketTime = g_Ticks;
            g_LastSendTime = g_LastPacketTime;
            if (GameVars::GetClientVersion() < CV_6060)
            {
                SendIP(m_LoginSocket, m_Seed);
            }
            else
            {
                u8 buf = 0xEF;
                m_LoginSocket.Send(&buf, 1); //0xEF
                SendIP(m_LoginSocket, m_Seed);
                Core::StreamWriter writer;
                u32 major = 0, minor = 0, rev = 0, prot = 0;
                GameVars::GetClientVersion(&major, &minor, &rev, &prot);
                writer.WriteBE<u32>(major);
                writer.WriteBE<u32>(minor);
                writer.WriteBE<u32>(rev);
                if (prot >= 'a')
                {
                    prot = 0;
                }
                writer.WriteBE<u32>(prot);

                g_TotalSendSize = 21;
                m_LoginSocket.Send(writer.GetData()); // Client version, 16 bytes
            }
        }
        else
        {
            m_LoginSocket.Disconnect();
        }
        return result;
    }

    if (m_GameSocket.IsConnected())
    {
        return true;
    }

    g_TotalSendSize = 4;
    g_LastPacketTime = g_Ticks;
    g_LastSendTime = g_LastPacketTime;

    const bool result = m_GameSocket.Connect(address, port);
    if (result)
    {
        SendIP(m_GameSocket, gameSeed);
    }

    m_LoginSocket.Disconnect();
    return result;

    return false;
}

void CConnectionManager::Disconnect()
{
    if (m_LoginSocket.IsConnected())
    {
        m_LoginSocket.Disconnect();
    }

    if (m_GameSocket.IsConnected())
    {
        m_GameSocket.Disconnect();
    }
}

void CConnectionManager::Recv()
{
    PROFILER_EVENT();
    if (m_IsLoginSocket)
    {
        if (!m_LoginSocket.IsConnected())
        {
            return;
        }

        if (!m_LoginSocket.ReadyRead())
        {
            if (m_LoginSocket.GetDataReady() == -1)
            {
                LOG_ERROR("ConnectionManager", "Failed to Recv()...Disconnecting...");
                g_Orion.InitScreen(GS_MAIN_CONNECT);
                g_ConnectionScreen.SetType(CST_CONLOST);
            }
            return;
        }

        g_PacketManager.Read(&m_LoginSocket);
    }
    else
    {
        if (!m_GameSocket.IsConnected())
        {
            return;
        }

        if (!m_GameSocket.ReadyRead())
        {
            if (m_GameSocket.GetDataReady() == -1)
            {
                LOG_ERROR("ConnectionManager", "Failed to Recv()...Disconnecting...");
                if (g_GameState == GS_GAME ||
                    (g_GameState == GS_GAME_BLOCKED && (g_GameBlockedScreen.Code != 0u)))
                {
                    g_Orion.DisconnectGump();
                }
                else
                {
                    g_Orion.InitScreen(GS_MAIN_CONNECT);
                    g_ConnectionScreen.SetType(CST_CONLOST);
                }
            }

            return;
        }

        g_PacketManager.Read(&m_GameSocket);
    }
}

int CConnectionManager::Send(u8 *buf, int size)
{
    if (g_Config.TheAbyss)
    {
        switch (buf[0])
        {
            case 0x34:
                buf[0] = 0x71;
                break;
            case 0x72:
                buf[0] = 0x6C;
                break;
            case 0x6C:
                buf[0] = 0x72;
                break;
            case 0x3B:
                buf[0] = 0x34;
                break;
            case 0x6F:
                buf[0] = 0x56;
                break;
            case 0x56:
                buf[0] = 0x6F;
                break;
            default:
                break;
        }
    }
    else if (g_Config.Asmut)
    {
        if (buf[0] == 0x02)
        {
            buf[0] = 0x04;
        }
        else if (buf[0] == 0x07)
        {
            buf[0] = 0x0A;
        }
    }

    if (m_IsLoginSocket)
    {
        if (!m_LoginSocket.IsConnected())
        {
            return 0;
        }

        std::vector<u8> cbuf(size);
        Crypt::Encrypt(true, &buf[0], &cbuf[0], size);
        return m_LoginSocket.Send(cbuf);
    }

    if (!m_GameSocket.IsConnected())
    {
        return 0;
    }

    std::vector<u8> cbuf(size);
    Crypt::Encrypt(false, &buf[0], &cbuf[0], size);
    return m_GameSocket.Send(cbuf);

    return 0;
}

int CConnectionManager::Send(const std::vector<u8> &data)
{
    return Send((u8 *)&data[0], (int)data.size());
}
