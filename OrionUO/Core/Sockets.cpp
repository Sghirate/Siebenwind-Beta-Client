#include "Sockets.h"
#include "Core/Log.h"
#include <assert.h>
#if defined(ORION_WINDOWS)
#include <winsock2.h>
#include <ws2tcpip.h>
//#pragma comment(lib, "wsock32.lib")
#pragma comment(lib, "ws2_32.lib")
#else
#include <arpa/inet.h>
#include <cstring>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
typedef struct hostent HOSTENT;
typedef HOSTENT* LPHOSTENT;
#define closesocket close
#define SOCKET int
#define SOCKADDR struct sockaddr
#define SOCKADDR_IN struct in_addr
#define LPIN_ADDR struct in_addr*
#define LPSOCKADDR SOCKADDR*
#define INVALID_SOCKET (-1)
#endif

namespace Core
{

struct ICMPHeader
{
    u8 type;
    u8 code;
    u16 checksum;
    u16 id;
    u16 seq;
};
struct ICMPEchoRequest
{
    enum
    {
        kType = 8
    };
    enum
    {
        kDataSize = 32
    };
    ICMPHeader icmpHeader;
    u8 data[kDataSize];
};
struct IPHeader
{
    unsigned char VIHL;
    unsigned char TOS;
    short TotLen;
    short ID;
    short FlagOff;
    unsigned char TTL;
    unsigned char Protocol;
    u16 Checksum;
    struct in_addr iaSrc;
    struct in_addr iaDst;
};
struct ICMPEchoReply
{
    enum
    {
        kPaddingSize = 256
    };
    IPHeader ipHeader;
    ICMPEchoRequest echoRequest;
    u8 padding[kPaddingSize];
};

static bool socket_init();
static u16 icmp_checksum(const u16* addr, int count)
{
    u16 checksum = 0;
    while (count > 1)
    {
        checksum += *addr++;
        count -= 2;
    }
    if (count > 0)
        checksum += *(unsigned char*)addr;
    while ((checksum >> 16) != 0)
        checksum = (checksum & 0xffff) + (checksum >> 16);
    return ~checksum;
}
#if defined(ORION_WINDOWS)
static bool wsa_initialized = false;
static bool socket_init()
{
    if (!wsa_initialized)
    {
        WSADATA wsaData = { 0 };
        wsa_initialized = !WSAStartup(MAKEWORD(2, 2), &wsaData);
    }
    return wsa_initialized;
}
#else
static bool socket_init()
{
    return true;
}
#endif

namespace Socket
{

    u32 GetLocalAddress()
    {
#if !defined(ORION_WINDOWS)
        struct sockaddr_in server_addr, my_addr;
        int s;
        if ((s = socket(AF_INET, SOCK_STREAM, 0)) >= 0)
        {
            bzero(&server_addr, sizeof(server_addr));
            server_addr.sin_family = AF_INET;
            server_addr.sin_addr.s_addr = inet_addr("1.1.1.1");
            server_addr.sin_port = htons(80);
            if (connect(s, (struct sockaddr *) &server_addr, sizeof(server_addr)) >= 0)
            {
                bzero(&my_addr, sizeof(my_addr));
                socklen_t len = sizeof(my_addr);
                getsockname(s, (struct sockaddr *) &my_addr, &len);
                return (u32)my_addr.sin_addr.s_addr;
            }
            closesocket(s);
        }
#endif
        char hostName[1024]{};
        if (gethostname(hostName, sizeof(hostName)) == 0)
        {
            LPHOSTENT lphost = gethostbyname(hostName);
            if (lphost)
            {
                return ((LPIN_ADDR)lphost->h_addr)->s_addr;
            }
        }
        return 0;
    }

    bool AddressToString(u32 a_addr, char* out_buffer, size_t a_bufferSize)
    {
        in_addr addr;
#if defined(ORION_WINDOWS)
        addr.S_un.S_addr = a_addr;
#else
        addr.s_addr = a_addr;
#endif
        memcpy(out_buffer, inet_ntoa(addr), a_bufferSize - 1);
        return inet_ntop(AF_INET, &addr, out_buffer, a_bufferSize) != nullptr;
    }

    u32 AddressFromString(const char* a_address)
    {
        sockaddr_in addr;
        memset(&addr, 0, sizeof(sockaddr_in));
        addr.sin_family = AF_INET;

        struct hostent* he;
        int rt = inet_addr(a_address);
        if (rt != -1)
        {
            addr.sin_addr.s_addr = rt;
        }
        else
        {
            he = gethostbyname(a_address);
            if (he == nullptr)
                return false;
            memcpy(&addr.sin_addr, he->h_addr, he->h_length);
        }

#if defined(ORION_WINDOWS)
        return addr.sin_addr.S_un.S_addr;
#else
        return addr.sin_addr.s_addr;
#endif
    }

    u16 ConvertPort(u16 a_port) { return htons(a_port); }

} // namespace Socket

TCPSocket::TCPSocket()
    : m_handle(INVALID_SOCKET)
{
}

TCPSocket::~TCPSocket()
{
    Close();
}

bool TCPSocket::Open()
{
    if (!socket_init())
        return false;

    m_handle = (THandle)socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    return m_handle != INVALID_SOCKET;
}

bool TCPSocket::Connect(const char* a_address, u16 a_port)
{
    SOCKET s = (SOCKET)m_handle;

    sockaddr_in addr;
    memset(&addr, 0, sizeof(sockaddr_in));
    addr.sin_family = AF_INET;

    struct hostent* he;
    int rt = inet_addr(a_address);
    if (rt != -1)
    {
        addr.sin_addr.s_addr = rt;
    }
    else
    {
        he = gethostbyname(a_address);
        if (he == nullptr)
            return false;
        memcpy(&addr.sin_addr, he->h_addr, he->h_length);
    }

    addr.sin_port = htons(a_port);
    bool success  = connect(s, (struct sockaddr*)&addr, sizeof(addr)) != -1;
    if (!success)
    {
        LOG_ERROR("TCPSocket", "Could not connect to: %s:%i", a_address, a_port);
#if defined(ORION_WINDOWS)
        auto e = WSAGetLastError();
        LOG_ERROR("TCPSocket", "%i", e);
    }
    WSASetLastError(0);
#else
    }
#endif
    return success;
}

int TCPSocket::Select()
{
    SOCKET s = (SOCKET)m_handle;
    fd_set rfds;
    struct timeval tv = { 0, 0 };
    FD_ZERO(&rfds);
    FD_SET(s, &rfds);
    return select((int)(s+1), &rfds, nullptr, nullptr, &tv);
}

int TCPSocket::Receive(u8* a_data, size_t a_size)
{
    SOCKET s = (SOCKET)m_handle;
    return recv(s, (char*)a_data, a_size, 0);
}

int TCPSocket::Send(u8* a_data, size_t a_size)
{
    SOCKET s = SOCKET(m_handle);
    return send(s, (char*)a_data, a_size, 0);
}

void TCPSocket::Close()
{
    if (m_handle != INVALID_SOCKET)
    {
        SOCKET s = SOCKET(m_handle);
        closesocket(s);
        m_handle = INVALID_SOCKET;
    }
}

bool TCPSocket::IsOpen() const
{
    return m_handle != INVALID_SOCKET;
}

ICMPSocket::ICMPSocket()
    : m_handle(INVALID_SOCKET)
{
}

ICMPSocket::~ICMPSocket()
{
    Close();
}

bool ICMPSocket::Open()
{
    if (!socket_init())
        return false;

    m_handle = (THandle)socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
#if defined(ORION_WINDOWS)
    if (m_handle == SOCKET_ERROR)
        m_handle = INVALID_SOCKET;
#endif // defined(ORION_WINDOWS)
    return m_handle != INVALID_SOCKET;
}

int ICMPSocket::Ping(const char* a_address, u8* a_data, size_t a_dataSize)
{
    SOCKET s = (SOCKET)m_handle;
    assert(s != INVALID_SOCKET);
    assert(a_address);
    assert(a_data == nullptr || a_dataSize <= sizeof(ICMPEchoRequest::kDataSize));

    LPHOSTENT lpHost = gethostbyname(a_address);
    if (lpHost != nullptr)
    {
        sockaddr_in destAddress;
        destAddress.sin_addr.s_addr = ((in_addr*)lpHost->h_addr_list[0])->s_addr;
        destAddress.sin_family      = AF_INET;
        destAddress.sin_port        = 0;

        ICMPEchoRequest request = {};
        request.icmpHeader.type = ICMPEchoRequest::kType;
        memset(request.data, 80, sizeof(ICMPEchoRequest::kDataSize));
        if (a_data)
            memcpy(&request.data, a_data, a_dataSize);

        const auto rs               = sizeof(ICMPEchoRequest);
        request.icmpHeader.checksum = icmp_checksum((u16*)&request, rs);

        const char* r  = (char*)&request;
        const auto dst = (LPSOCKADDR)&destAddress;
        ::sendto(s, r, rs, 0, dst, sizeof(SOCKADDR_IN));

        timeval timeoutInfo;
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(s, &readfds);
        timeoutInfo.tv_sec  = 1;
        timeoutInfo.tv_usec = 0;

        if (::select(1, &readfds, nullptr, nullptr, &timeoutInfo))
        {
            ICMPEchoReply reply;
            sockaddr_in sourceAddress;
            int length     = sizeof(sockaddr_in);
            char* a        = (char*)&reply;
            auto as        = sizeof(ICMPEchoReply);
            auto src       = (LPSOCKADDR)&sourceAddress;
            socklen_t* len = (socklen_t*)&length;
            if (::recvfrom(s, a, as, 0, src, len) != -1)
            {
                if (a_data)
                    memcpy(a_data, &reply.echoRequest.data, a_dataSize);
                return 0;
            }
            else
            {
                return -1;
            }
        }
        return -2;
    }
    return -3;
}

void ICMPSocket::Close()
{
    if (m_handle != INVALID_SOCKET)
    {
        SOCKET s = (SOCKET)m_handle;
        closesocket(s);
        m_handle = INVALID_SOCKET;
    }
}

} // namespace Core