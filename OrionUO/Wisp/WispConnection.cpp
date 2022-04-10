

#include "../Definitions.h"
#include "WispConnection.h"
#include "WispPacketMessage.h"

namespace Wisp
{
CConnection::CConnection()
{
}

void CConnection::Init()
{

    m_Socket = tcp_open();
    m_MessageParser = new CPacketMessage();
}

CConnection::~CConnection()
{
    Disconnect();

    if (m_MessageParser != nullptr)
    {
        delete m_MessageParser;
        m_MessageParser = nullptr;
    }
}

bool CConnection::Connect(const std::string &address, u16 port)
{
    if (Connected)
    {
        return false;
    }

    if (m_Socket == nullptr)
    {
        m_Socket = tcp_open();
    }

    if (m_Socket == nullptr)
    {
        return false;
    }

    if (!tcp_connect(m_Socket, address.c_str(), port))
    {
        return false;
    }

    Port = port;
    Connected = true;
    m_MessageParser->Clear();

    return true;
}

void CConnection::Disconnect()
{
    if (Connected && m_Socket != nullptr)
    {
        tcp_close(m_Socket);
        m_Socket = nullptr;
        Connected = false;
        DataReady = 0;
        Port = 0;
        m_MessageParser->Clear();
    }
}

bool CConnection::ReadyRead()
{
    if (!Connected || m_Socket == nullptr)
    {
        return false;
    }

    DataReady = tcp_select(m_Socket);
    if (DataReady == -1)
    {
        LOG("CConnection::ReadyRead SOCKET_ERROR\n");
        Disconnect();
    }

    return (DataReady != 0);
}

bool CConnection::Read(int maxSize)
{
    if (DataReady == -1)
    {
        LOG("CConnection::Read, m_DataReady=%i\n", DataReady);
        Disconnect();
    }
    else if (Connected && m_Socket != nullptr)
    {
        std::vector<u8> data(maxSize);
        const int size = tcp_recv(m_Socket, &data[0], maxSize);

        if (size > 0)
        {
            LOG("CConnection::Read size=%i\n", size);
            data.resize(size);
            data = Decompression(data);
            m_MessageParser->Append(data);
            return true;
        }

        LOG("CConnection::Read, bad size=%i\n", size);
    }
    else
    {
        LOG("CConnection::Read, unknown state, m_Connected=%i\n", Connected);
    }

    return false;
}

int CConnection::Send(u8 *data, int size)
{
    if (!Connected || m_Socket == nullptr)
    {
        return 0;
    }

    const int sent = tcp_send(m_Socket, data, size);
    //LOG("CConnection::Send=>%i\n", sent);
    return sent;
}

int CConnection::Send(const std::vector<u8> &data)
{
    if (data.empty())
    {
        return 0;
    }

    const int sent = Send((u8 *)&data[0], (int)data.size());
    LOG("CConnection::Send=>%i\n", sent);
    return sent;
}
}; // namespace Wisp
