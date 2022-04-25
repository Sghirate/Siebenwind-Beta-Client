#pragma once

#include "Core/Connection.h"
#include "UOHuffman.h"
#include <string>
#include <vector>

class CSocket : public Core::Connection
{
public:
    bool GameSocket = false;
    bool UseProxy = false;
    std::string ProxyAddress = "";
    u16 ProxyPort = 0;
    bool ProxySocks5 = false;
    std::string ProxyAccount = "";
    std::string ProxyPassword = "";

private:
    CDecompressingCopier m_Decompressor;

public:
    CSocket(bool gameSocket);
    ~CSocket();

    virtual bool Connect(const std::string &address, u16 port);
    virtual std::vector<u8> Decompression(std::vector<u8> data);
};
