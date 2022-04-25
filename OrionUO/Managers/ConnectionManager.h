#pragma once

#include "../Network/Connection.h"

class CConnectionManager
{
protected:
    bool m_UseProxy = false;

public:
    bool GetUseProxy() { return m_UseProxy; };
    void SetUseProxy(bool val);

protected:
    string m_ProxyAddress = "";

public:
    string GetProxyAddress() { return m_ProxyAddress; };
    void SetProxyAddress(const std::string &val);

protected:
    int m_ProxyPort = 0;

public:
    int GetProxyPort() { return m_ProxyPort; };
    void SetProxyPort(int val);

protected:
    bool m_ProxySocks5 = false;

public:
    bool GetProxySocks5() { return m_ProxySocks5; };
    void SetProxySocks5(bool val);

protected:
    string m_ProxyAccount = "";

public:
    string GetProxyAccount() { return m_ProxyAccount; };
    void SetProxyAccount(const std::string &val);

protected:
    string m_ProxyPassword = "";

public:
    string GetProxyPassword() { return m_ProxyPassword; };
    void SetProxyPassword(const std::string &val);

private:
    CSocket m_LoginSocket{ CSocket(false) };
    CSocket m_GameSocket{ CSocket(true) };
    // true - m_LoginSocket, false - m_GameSocket
    bool m_IsLoginSocket = true;
    u8 m_Seed[4];

    void SendIP(CSocket &socket, u8 *ip);

public:
    CConnectionManager();
    ~CConnectionManager();

    void Init();
    void Init(u8 *gameSeed);
    bool Connected() { return (m_LoginSocket.IsConnected() || m_GameSocket.IsConnected()); }
    bool Connect(const std::string &address, int port, u8 *gameSeed);
    void Disconnect();
    void Recv();
    int Send(u8 *buf, int size);
    int Send(const std::vector<u8> &data);
    const u8 *GetClientIP() const { return &m_Seed[0]; }
};

extern CConnectionManager g_ConnectionManager;
