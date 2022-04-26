#pragma once

#include "Core/Minimal.h"

namespace Core
{

namespace Socket
{
    u32 GetLocalAddress();
    bool AddressToString(u32 a_addr, char* out_buffer, size_t a_bufferSize);
    template<size_t SIZE>
    inline bool AddressToString(u32 a_addr, char(&out_buffer)[SIZE]) { return AddressToString(a_addr, out_buffer, SIZE); }
    u32 AddressFromString(const char* a_address);
    u16 ConvertPort(u16 a_port);
}
struct TCPSocket
{
    typedef u64 THandle;

    TCPSocket();
    ~TCPSocket();

    bool Open();
    bool Connect(const char* a_address, u16 a_port);
    int Select();
    int Receive(u8* a_data, size_t a_size);
    int Send(u8* a_data, size_t a_size);
    void Close();
    bool IsOpen() const;

    template<size_t SIZE>
    inline int Receive(u8(&a_buffer)[SIZE]) { return Receive(a_buffer, SIZE); }
    template<size_t SIZE>
    inline int Send(u8(&a_buffer)[SIZE]) { return Send(a_buffer, SIZE); }

private:
    THandle m_handle;
};

struct ICMPSocket
{
    typedef u64 THandle;

    ICMPSocket();
    ~ICMPSocket();

    bool Open();
    int Ping(const char* a_address, u8* a_data, size_t a_dataSize);
    void Close();

    template<size_t SIZE>
    inline int Ping(const char* a_address, u8(&a_buffer)[SIZE]) { return Ping(a_address, a_buffer, SIZE); }
    template<typename TData>
    inline int Ping(const char* a_address, TData& a_data) { return Ping(a_address, &a_data, sizeof(TData)); }
    inline int Ping(const char* a_address) { return Ping(a_address, nullptr, 0); }

private:
    THandle m_handle;
};

} // namespace Core
