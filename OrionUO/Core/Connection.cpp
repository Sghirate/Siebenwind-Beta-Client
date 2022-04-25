#include "Connection.h"
#include "Core/Log.h"
#include "Core/PacketMessage.h"

namespace Core
{

Connection::Connection()
{
}

void Connection::Init()
{
    m_socket = tcp_open();
    m_messageParser = new PacketMessage();
}

Connection::~Connection()
{
    Disconnect();

    if (m_messageParser != nullptr)
    {
        delete m_messageParser;
        m_messageParser = nullptr;
    }
}

bool Connection::Connect(const std::string& a_address, u16 a_port)
{
    if (m_connected)
    {
        return false;
    }

    if (m_socket == nullptr)
    {
        m_socket = tcp_open();
    }

    if (m_socket == nullptr)
    {
        return false;
    }

    if (!tcp_connect(m_socket, a_address.c_str(), a_port))
    {
        return false;
    }

    m_port = a_port;
    m_connected = true;
    m_messageParser->Clear();

    return true;
}

void Connection::Disconnect()
{
    if (m_connected && m_socket != nullptr)
    {
        tcp_close(m_socket);
        m_socket = nullptr;
        m_connected = false;
        m_dataReady = 0;
        m_port = 0;
        m_messageParser->Clear();
    }
}

bool Connection::ReadyRead()
{
    if (!m_connected || m_socket == nullptr)
    {
        return false;
    }

    m_dataReady = tcp_select(m_socket);
    if (m_dataReady == -1)
    {
        LOG_ERROR("Connection", "ReadyRead: SOCKET_ERROR");
        Disconnect();
    }

    return (m_dataReady != 0);
}

bool Connection::Read(int a_maxSize)
{
    if (m_dataReady == -1)
    {
        LOG_ERROR("Connection", "Read, m_DataReady=%i", m_dataReady);
        Disconnect();
    }
    else if (m_connected && m_socket != nullptr)
    {
        std::vector<u8> data(a_maxSize);
        const int size = tcp_recv(m_socket, &data[0], a_maxSize);

        if (size > 0)
        {
            LOG_INFO("Connection", "Read size=%i", size);
            data.resize(size);
            data = Decompress(data);
            m_messageParser->Append(data);
            return true;
        }

        LOG_ERROR("Connection", "Read, bad size=%i", size);
    }
    else
    {
        LOG_ERROR("Connection", "Read, unknown state, m_Connected=%i", m_connected);
    }

    return false;
}

int Connection::Send(u8* a_data, int a_size)
{
    if (!m_connected || m_socket == nullptr)
    {
        return 0;
    }

    const int sent = tcp_send(m_socket, a_data, a_size);
    //LOG_F(INFO, "Connection::Send=>%i\n", sent);
    return sent;
}

int Connection::Send(const std::vector<u8>& a_data)
{
    if (a_data.empty())
    {
        return 0;
    }

    const int sent = Send((u8*)&a_data[0], (int)a_data.size());
    LOG_INFO("Connection", "Send=>%i", sent);
    return sent;
}

} // namespace Core
