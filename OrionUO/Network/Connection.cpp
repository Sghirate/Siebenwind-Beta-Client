#include "Connection.h"
#include "Core/Log.h"
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
            m_socket.Close();
            m_connected = false;
            LOG_INFO("Socket", "Connecting...%s:%i", address.c_str(), port);
            return Core::Connection::Connect(address, port);
        }

        u16 serverPort = Core::Socket::ConvertPort(port);
        u32 serverIP = Core::Socket::AddressFromString(address.c_str());

        if (serverIP == 0xFFFFFFFF)
        {
            LOG_WARNING("Socket", "Unknowm server address");
            m_socket.Close();
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
            m_socket.Send(str, 4);
            int num =  m_socket.Receive(str);
            if ((str[0] != 5) || (num != 2))
            {
                LOG_WARNING("Socket", "Proxy Server Version Missmatch");
                m_socket.Close();
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
                    m_socket.Send((unsigned char *)&buffer[0], totalSize);
                    m_socket.Receive(str);
                    if (str[1] != 0)
                    {
                        LOG_WARNING("Socket", "Wrong Username/Password");
                        m_socket.Close();
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
                m_socket.Send(str, 10);
                num = m_socket.Receive(str);
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
                    m_socket.Close();
                    m_connected = false;
                    LOG_INFO("Socket", "Connecting...%s:%i", address.c_str(), port);
                    return Core::Connection::Connect(address, port);
                }
                LOG_INFO("Socket", "Connected to server via proxy");
            }
            else
            {
                LOG_WARNING("Socket", "No acceptable methods");
                m_socket.Close();
                m_connected = false;
                LOG_INFO("Socket", "Connecting...%s:%i", address.c_str(), port);
                return Core::Connection::Connect(address, port);
            }
        }
        else
        {
            LOG_INFO("Socket", "Proxy Server Version 4 Selected");
            unsigned char str[9] = { 0 };
            str[0] = 4;
            str[1] = 1;
            memcpy(&str[2], &serverPort, 2);
            memcpy(&str[4], &serverIP, 4);
            m_socket.Send(str);
            int recvSize = m_socket.Receive(str, 8);
            if ((recvSize != 8) || (str[0] != 0) || (str[1] != 90))
            {
                if (str[0] == 5)
                {
                    LOG_INFO("Socket", "Proxy Server Version is 5");
                    LOG_INFO("Socket", "Trying  SOCKS5");
                    m_socket.Close();
                    m_connected = false;
                    ProxySocks5 = true;
                    return Connect(address, port);
                }
                switch (str[1])
                {
                    case 1:
                    case 91:
                        LOG_ERROR("Socket", "Proxy request rejected or failed");
                        break;
                    case 2:
                    case 92:
                        LOG_ERROR("Socket", "Proxy rejected becasue SOCKS server cannot connect to identd on the client");
                        break;
                    case 3:
                    case 93:
                        LOG_ERROR("Socket", "Proxy rejected becasue SOCKS server cannot connect to identd on the client");
                        break;
                    default:
                        LOG_ERROR("Socket", "Unknown Error <%d> recieved", str[1]);
                        break;
                }
                m_socket.Close();
                m_connected = false;
                LOG_INFO("Socket", "Connecting...%s:%i", address.c_str(), port);
                return Core::Connection::Connect(address, port);
            }
            LOG_INFO("Socket", "Connected to server via proxy");
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
            LOG_ERROR("Socket", "decompression buffer too small");
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
