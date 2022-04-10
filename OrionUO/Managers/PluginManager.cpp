// MIT License
// Copyright (C) September 2016 Hotride

#include "PluginManager.h"
#include "../Platform.h"
#include "plugin/commoninterfaces.h"

#include "PacketManager.h"
#include "ConnectionManager.h"
#include "../Config.h"
#include "../OrionWindow.h"

CPluginManager g_PluginManager;

bool CDECL PluginRecvFunction(u8 *buf, size_t size)
{
    auto owned = (u8 *)malloc(size);
    memcpy(owned, buf, size);
    PUSH_EVENT(UOMSG_RECV, owned, size);
    return true;
}

bool CDECL PluginSendFunction(u8 *buf, size_t size)
{

    auto owned = (u8 *)malloc(size);
    memcpy(owned, buf, size);
    PUSH_EVENT(UOMSG_SEND, owned, size);
    u32 ticks = g_Ticks;
    g_TotalSendSize += checked_cast<int>(size);
    CPacketInfo &type = g_PacketManager.GetInfo(*buf);
    LOG("--- ^(%d) s(+%zd => %d) Plugin->Server:: %s\n",
        ticks - g_LastPacketTime,
        size,
        g_TotalSendSize,
        type.Name);

    g_LastPacketTime = ticks;
    g_LastSendTime = ticks;
    if (*buf == 0x80 || *buf == 0x91)
    {
        LOG_DUMP(buf, 1);
        SAFE_LOG_DUMP(buf, int(size));
        LOG("**** ACCOUNT AND PASSWORD CENSORED ****\n");
    }
    else
    {
        LOG_DUMP(buf, int(size));
    }

    g_ConnectionManager.Send(buf, checked_cast<int>(size));
    return true;
}

CPlugin::CPlugin(u32 flags)
    : m_Flags(flags)
{
    m_PPS = new PLUGIN_INTERFACE();
    memset(m_PPS, 0, sizeof(PLUGIN_INTERFACE));
    m_PPS->Handle = g_OrionWindow.Handle;
    m_PPS->ClientVersion = g_Config.ClientVersion;
    m_PPS->ClientFlags = (g_Config.UseVerdata ? 0x01 : 0);
}

CPlugin::~CPlugin()
{
    if (m_PPS != nullptr)
    {
        delete m_PPS;
        m_PPS = nullptr;
    }
}

CPluginManager::CPluginManager()
{
}

u32 CPluginManager::OnEvent(u32 msg, const void *data)
{

    u32 result = 0;
    QFOR(plugin, m_Items, CPlugin *)
    {
        if (plugin->CanEvent() && plugin->m_PPS->OnEvent != nullptr)
        {
            result = plugin->m_PPS->OnEvent(msg, data);
        }
    }
    return result;
}

bool CPluginManager::PacketRecv(u8 *buf, size_t size)
{

    bool result = true;
    QFOR(plugin, m_Items, CPlugin *)
    {
        if (plugin->CanParseRecv() && plugin->m_PPS->OnRecv != nullptr)
        {
            bool funRet = plugin->m_PPS->OnRecv(buf, size);
            if (result)
            {
                result = funRet;
            }
        }
    }
    return result;
}

bool CPluginManager::PacketSend(u8 *buf, size_t size)
{

    bool result = true;
    QFOR(plugin, m_Items, CPlugin *)
    {
        if (plugin->CanParseSend() && plugin->m_PPS->OnSend != nullptr)
        {
            bool funRet = plugin->m_PPS->OnSend(buf, size);
            if (result)
            {
                result = funRet;
            }
        }
    }
    return result;
}

void CPluginManager::Disconnect()
{

    QFOR(plugin, m_Items, CPlugin *)
    {
        if (plugin->m_PPS->OnDisconnect != nullptr)
        {
            plugin->m_PPS->OnDisconnect();
        }
    }
}

void CPluginManager::WorldDraw()
{

    QFOR(plugin, m_Items, CPlugin *)
    {
        if (plugin->CanEnterWorldRender() && plugin->m_PPS->OnWorldDraw != nullptr)
        {
            plugin->m_PPS->OnWorldDraw();
        }
    }
}

void CPluginManager::SceneDraw()
{

    QFOR(plugin, m_Items, CPlugin *)
    {
        if (plugin->CanEnterSceneRender() && plugin->m_PPS->OnSceneDraw != nullptr)
        {
            plugin->m_PPS->OnSceneDraw();
        }
    }
}

void CPluginManager::WorldMapDraw()
{

    QFOR(plugin, m_Items, CPlugin *)
    {
        if (plugin->CanEnterWorldMapRender() && plugin->m_PPS->OnWorldMapDraw != nullptr)
        {
            plugin->m_PPS->OnWorldMapDraw();
        }
    }
}
