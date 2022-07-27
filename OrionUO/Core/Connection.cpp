#include "Connection.h"
#include "Core/Console.h"
#include "Core/Log.h"
#include "Core/PacketMessage.h"

namespace
{
static const char* LCAT = "Core::Connection";
Core::ConsoleVariable<int> dbg_net_packets("dbg_net_packets", 0);
static void LogNetData(const char* a_prefix, const u8* a_data, const int a_size)
{
    if (dbg_net_packets.GetValue() > 0)
    {
        if (a_size > 0)
        {
            static char pktbuf[512];
            int n = a_size >= 256 ? (256-3) : a_size;
            char* buf = pktbuf;
            for (int i = 0; i < n; ++i)
            {
                buf += sprintf(buf, "%02X", a_data[i]);
            }
            if (a_size > n)
            {
                buf += sprintf(buf, "...");
            }
            *buf = '\0';
            LOG_INFO(LCAT, "%s: %s, %i", a_prefix, pktbuf, a_size);
        }
        else
        {
            LOG_INFO(LCAT, "%s: - (0)", a_prefix);
        }
    }
}
}

namespace Core
{

Connection::Connection()
{
}

void Connection::Init()
{
    m_socket.Open();
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
        return false;

    if (!m_socket.IsOpen() && !m_socket.Open())
        return false;

    if (!m_socket.Connect(a_address.c_str(), a_port))
        return false;

    m_port = a_port;
    m_connected = true;
    m_messageParser->Clear();
    return true;
}

void Connection::Disconnect()
{
    if (m_connected)
    {
        m_connected = false;
        m_dataReady = 0;
        m_port = 0;
        m_messageParser->Clear();
    }
    if (m_socket.IsOpen())
    {
        m_socket.Close();
    }
}

bool Connection::ReadyRead()
{
    if (!m_connected || !m_socket.IsOpen())
    {
        return false;
    }

    m_dataReady = m_socket.Select();
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
    else if (m_connected && m_socket.IsOpen())
    {
        std::vector<u8> data(a_maxSize);
        const int size = m_socket.Receive(&data[0], a_maxSize);
        LogNetData("Recv", data.data(), size);
        if (size > 0)
        {
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
    LogNetData("Send", a_data, a_size);
    return (m_connected && m_socket.IsOpen()) ? m_socket.Send(a_data, a_size) : 0;
}

int Connection::Send(const std::vector<u8>& a_data)
{
    return !a_data.empty() ? Send((u8*)&a_data[0], (int)a_data.size()) : 0;
}

} // namespace Core
