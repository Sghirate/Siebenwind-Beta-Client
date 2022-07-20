#pragma once

#include "Core/Minimal.h"
#include "Core/Sockets.h"
#include <string>
#include <vector>

namespace Core
{

struct Connection
{
    Connection();
    virtual ~Connection();
    void Init();

    virtual bool Connect(const std::string& a_address, u16 a_port);
    void Disconnect();

    bool ReadyRead();
    bool IsConnected() const { return m_connected; }
    int GetDataReady() const { return m_dataReady; }
    struct PacketMessage* GetMessageParser() const { return m_messageParser; }

    virtual std::vector<u8> Decompress(std::vector<u8> a_data) { return a_data; }

    bool Read(int a_maxSize = 0x1000);

    int Send(u8* a_data, int size);
    int Send(const std::vector<u8>& data);

protected:
    struct PacketMessage* m_messageParser;
    TCPSocket m_socket;
    int m_dataReady = 0;
    int m_port = 0;
    bool m_connected = 0;
};

} // namespace Core
