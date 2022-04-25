#include "Connection.h"
#include "Core/Log.h"
#include "../Sockets.h"
#include "../Crypt/CryptEntry.h"

CSocket::CSocket(bool gameSocket)
    : GameSocket(gameSocket)
{
}

CSocket::~CSocket()
{
}

bool CSocket::Connect(const std::string &address, u16 port)
{
    LOG_INFO("Socket", "Connecting...%s:%i", address.c_str(), port);

    if (UseProxy)
    {
        if (IsConnected())
        {
            return false;
        }
        LOG_INFO("Socket", "Connecting using proxy %s:%d", ProxyAddress.c_str(), ProxyPort);
        if (!Core::Connection::Connect(ProxyAddress, ProxyPort))
        {
            LOG_WARNING("Socket", "Can't connect to proxy");
            m_socket = nullptr;
            Connected = false;
            LOG_INFO("Socket", "Connecting...%s:%i", address.c_str(), port);
            return Core::Connection::Connect(address, port);
        }

        u16 serverPort = htons(port);
        u32 serverIP = inet_addr(address.c_str());

        if (serverIP == 0xFFFFFFFF)
        {
            struct hostent *uohe = gethostbyname(address.c_str());

            if (uohe != nullptr)
            {
                sockaddr_in caddr;
                memcpy(&caddr.sin_addr, uohe->h_addr, uohe->h_length);
#if defined(ORION_WINDOWS)
                serverIP = caddr.sin_addr.S_un.S_addr;
#else
                serverIP = caddr.sin_addr.s_addr;
#endif
            }
        }

        if (serverIP == 0xFFFFFFFF)
        {
            LOG_WARNING("Socket", "Unknowm server address");
            tcp_close(m_socket);
            m_socket = nullptr;
            m_connected = false;
            LOG_INFO("Socket", "Connecting...%s:%i", address.c_str(), port);
            return Core::Connection::Connect(address, port);
        }

        if (ProxySocks5)
        {
            LOG_INFO("Socket", "Proxy Server Version 5 Selected");
            unsigned char str[255] = { 0 };
            str[0] = 5; //Proxy Version
            str[1] = 2; //Number of authentication method
            str[2] = 0; //No auth required
            str[3] = 2; //Username/Password auth
            tcp_send(m_socket, str, 4);
            int num = tcp_recv(m_socket, str, 255);
            if ((str[0] != 5) || (num != 2))
            {
                LOG_WARNING("Socket", "Proxy Server Version Missmatch");
                tcp_close(m_socket);
                m_socket = nullptr;
                m_connected = false;
                LOG_INFO("Socket", "Connecting...%s:%i", address.c_str(), port);
                return Core::Connection::Connect(address, port);
            }

            if ((str[1] == 0) || (str[1] == 2))
            {
                if (str[1] == 2)
                {
                    LOG_INFO("Socket", "Proxy wants Username/Password");
                    int totalSize = 3 + (int)ProxyAccount.length() + (int)ProxyPassword.length();
                    std::vector<char> buffer(totalSize, 0);
                    sprintf(&buffer[0], "  %s %s", ProxyAccount.c_str(), ProxyPassword.c_str());
                    buffer[0] = 1;
                    buffer[1] = (char)ProxyAccount.length();
                    buffer[2 + (int)ProxyAccount.length()] = (char)ProxyPassword.length();
                    tcp_send(m_socket, (unsigned char *)&buffer[0], totalSize);
                    tcp_recv(m_socket, str, 255);
                    if (str[1] != 0)
                    {
                        LOG_WARNING("Socket", "Wrong Username/Password");
                        tcp_close(m_socket);
                        m_socket = nullptr;
                        m_connected = false;
                        LOG_INFO("Socket", "Connecting...%s:%i", address.c_str(), port);
                        return Core::Connection::Connect(address, port);
                    }
                }
                memset(str, 0, 10);
                str[0] = 5;
                str[1] = 1;
                str[2] = 0;
                str[3] = 1;
                memcpy(&str[4], &serverIP, 4);
                memcpy(&str[8], &serverPort, 2);
                tcp_send(m_socket, str, 10);
                num = tcp_recv(m_socket, str, 255);
                if (str[1] != 0)
                {
                    switch (str[1])
                    {
                        case 1:
                            LOG_ERROR("Socket", "general SOCKS server failure");
                            break;
                        case 2:
                            LOG_ERROR("Socket", "connection not allowed by ruleset");
                            break;
                        case 3:
                            LOG_ERROR("Socket", "Network unreachable");
                            break;
                        case 4:
                            LOG_ERROR("Socket", "Host unreachable");
                            break;
                        case 5:
                            LOG_ERROR("Socket", "Connection refused");
                            break;
                        case 6:
                            LOG_ERROR("Socket", "TTL expired");
                            break;
                        case 7:
                            LOG_ERROR("Socket", "Command not supported");
                            break;
                        case 8:
                            LOG_ERROR("Socket", "Address type not supported");
                            break;
                        case 9:
                            LOG_ERROR("Socket", "to X'FF' unassigned");
                            break;
                        default:
                            LOG_ERROR("Socket", "Unknown Error <%d> recieved", str[1]);
                    }

                    tcp_close(m_socket);
                    m_socket = nullptr;
                    Connected = false;
                    LOG("Connecting...%s:%i\n", address.c_str(), port);
                    return Core::Connection::Connect(address, port);
                }
                LOG("Connected to server via proxy\n");
            }
            else
            {
                LOG("No acceptable methods\n");
                tcp_close(m_socket);
                m_socket = nullptr;
                Connected = false;
                LOG("Connecting...%s:%i\n", address.c_str(), port);
                return Core::Connection::Connect(address, port);
            }
        }
        else
        {
            LOG("Proxy Server Version 4 Selected\n");
            unsigned char str[9] = { 0 };
            str[0] = 4;
            str[1] = 1;
            memcpy(&str[2], &serverPort, 2);
            memcpy(&str[4], &serverIP, 4);
            tcp_send(m_socket, str, 9);
            int recvSize = tcp_recv(m_socket, str, 8);
            if ((recvSize != 8) || (str[0] != 0) || (str[1] != 90))
            {
                if (str[0] == 5)
                {
                    LOG("Proxy Server Version is 5\n");
                    LOG("Trying  SOCKS5\n");
                    tcp_close(m_socket);
                    m_socket = nullptr;
                    Connected = false;
                    ProxySocks5 = true;
                    return Connect(address, port);
                }
                switch (str[1])
                {
                    case 1:
                    case 91:
                        LOG("Proxy request rejected or failed\n");
                        break;
                    case 2:
                    case 92:
                        LOG("Proxy rejected becasue SOCKS server cannot connect to identd on the client\n");
                        break;
                    case 3:
                    case 93:
                        LOG("Proxy rejected becasue SOCKS server cannot connect to identd on the client\n");
                        break;
                    default:
                        LOG("Unknown Error <%d> recieved\n", str[1]);
                        break;
                }
                tcp_close(m_socket);
                m_socket = nullptr;
                Connected = false;
                LOG("Connecting...%s:%i\n", address.c_str(), port);
                return Core::Connection::Connect(address, port);
            }
            LOG("Connected to server via proxy\n");
        }
    }
    else
    {
        return Core::Connection::Connect(address, port);
    }

    return true;
}

std::vector<u8> CSocket::Decompression(std::vector<u8> data)
{
    if (GameSocket)
    {
        auto inSize = (intptr_t)data.size();
        Crypt::Decrypt(&data[0], &data[0], (int)inSize);

        std::vector<u8> decBuf(inSize * 4 + 2);
        int outSize = 65536;
        m_Decompressor((char *)&decBuf[0], (char *)&data[0], outSize, inSize);
        if (inSize != data.size())
        {
            DebugMsg("decompression buffer too small\n");
            Disconnect();
        }
        else
        {
            decBuf.resize(outSize);
        }
        return decBuf;
    }
    return data;
}
