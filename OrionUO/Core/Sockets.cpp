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
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
typedef struct hostent HOSTENT;
typedef HOSTENT* LPHOSTENT;
#define SOCKADDR struct sockaddr
#define SOCKADDR_IN struct in_addr
#define LPIN_ADDR struct in_addr*
#define LPSOCKADDR const SOCKADDR*
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
    enum { kType = 8 };
    enum { kDataSize = 32 };
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
    enum { kPaddingSize = 256 };
    IPHeader ipHeader;
    ICMPEchoRequest echoRequest;
    u8 padding[kPaddingSize];
};

static bool socket_init();
template <typename T>
static SOCKET& socket_cast(T a_handle)
{
    static_assert(sizeof(T) >= sizeof(SOCKET));
    return reinterpret_cast<SOCKET&>(a_handle);
}
static u16 icmp_checksum(const u16 *addr, int count)
{
    u16 checksum = 0;
    while (count > 1)
    {
        checksum += *addr++;
        count -= 2;
    }
    if (count > 0)
        checksum += *(unsigned char *)addr;
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
    char hostName[1024]{};
    if (gethostname(hostName, sizeof(hostName)) != 0)
        return 0;

    LPHOSTENT lphost = gethostbyname(hostName);
    if (!lphost)
        return 0;

    return ((LPIN_ADDR)lphost->h_addr)->s_addr;
}

bool AddressToString(u32 a_addr, char* out_buffer, size_t a_bufferSize)
{
    in_addr addr;
#if defined(ORION_WINDOWS)
    addr.S_un.S_addr = a_addr;
#else
    addr.s_addr = a_addr;
#endif
    memcpy(out_buffer, inet_ntoa(addr), a_bufferSize-1);
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

u16 ConvertPort(u16 a_port)
{
    return htons(a_port);
}

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
    SOCKET& s = socket_cast(m_handle);
    s         = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    return s != INVALID_SOCKET;
}

bool TCPSocket::Connect(const char* a_address, u16 a_port)
{
    SOCKET& s = socket_cast(m_handle);

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
    }
    WSASetLastError(0);
#else
    }
#endif
    return success;
}

int TCPSocket::Select()
{
    SOCKET& s = socket_cast(m_handle);
    fd_set rfds;
    struct timeval tv = { 0, 0 };
    FD_ZERO(&rfds);
    FD_SET(s, &rfds);
    return select((int)s, &rfds, nullptr, nullptr, &tv);
}

int TCPSocket::Receive(u8* a_data, size_t a_size)
{
    SOCKET& s = socket_cast(m_handle);
    return recv(s, (char*)a_data, a_size, 0);
}

int TCPSocket::Send(u8* a_data, size_t a_size)
{
    SOCKET& s = socket_cast(m_handle);
    return send(s, (char *)a_data, a_size, 0);
}

void TCPSocket::Close()
{
    SOCKET& s = socket_cast(m_handle);
    if (s != INVALID_SOCKET)
    {
        closesocket(s);
        s = INVALID_SOCKET;
    }
}

bool TCPSocket::IsOpen() const
{
    const SOCKET& s = socket_cast(m_handle);
    return s != INVALID_SOCKET;
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
    if(!socket_init())
        return false;

    SOCKET& s = socket_cast(m_handle);
    s = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (s == SOCKET_ERROR)
        s = INVALID_SOCKET;
    return s != INVALID_SOCKET;
}

int ICMPSocket::Ping(const char* a_address, u8* a_data, size_t a_dataSize)
{
    SOCKET& s = socket_cast(m_handle);
    assert(s != INVALID_SOCKET);
    assert(a_address);
    assert(a_data == nullptr || a_dataSize <= sizeof(ICMPEchoRequest::kDataSize));

    LPHOSTENT lpHost = gethostbyname(a_address);
    if (lpHost != nullptr)
    {
        sockaddr_in destAddress;
        destAddress.sin_addr.s_addr = ((in_addr *)lpHost->h_addr_list[0])->s_addr;
        destAddress.sin_family = AF_INET;
        destAddress.sin_port = 0;

        ICMPEchoRequest request = {};
        request.icmpHeader.type = ICMPEchoRequest::kType;
        memset(request.data, 80, sizeof(ICMPEchoRequest::kDataSize));
        if (a_data)
            memcpy(&request.data, a_data, a_dataSize);

        const auto rs = sizeof(ICMPEchoRequest);
        request.icmpHeader.checksum = icmp_checksum((u16 *)&request, rs);

        const auto r = (LPSTR)&request;
        const auto dst = (LPSOCKADDR)&destAddress;
        ::sendto(s, r, rs, 0, dst, sizeof(SOCKADDR_IN));

        timeval timeoutInfo;
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(s, &readfds);
        timeoutInfo.tv_sec = 1;
        timeoutInfo.tv_usec = 0;

        if (::select(1, &readfds, nullptr, nullptr, &timeoutInfo))
        {
            ICMPEchoReply reply;
            sockaddr_in sourceAddress;
            int length = sizeof(sockaddr_in);
            const auto a = (LPSTR)&reply;
            const auto as = sizeof(ICMPEchoReply);
            const auto src = (LPSOCKADDR)&sourceAddress;
            if (::recvfrom(s, a, as, 0, src, &length) != -1)
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
    SOCKET& s = socket_cast(m_handle);
    if (s != INVALID_SOCKET)
    {
        closesocket(s);
        s = INVALID_SOCKET;
    }
}


} // namespace Core