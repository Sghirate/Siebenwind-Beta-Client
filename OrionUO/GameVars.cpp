#include "GameVars.h"
#include "Config.h"
#include "plugin/enumlist.h"
#include <cstring>

namespace 
{
u32 g_clientVersion = CV_LATEST;
static u32 ClientVersionFromString(const std::string& a_string)
{
    if (a_string.empty())
        return CV_LATEST;
    int a = 0, b = 0, c = 0, d = 0;
    char extra[16]{};
    char tok = 0;
    sscanf(a_string.c_str(), "%d.%d.%d%s", &a, &b, &c, extra);
    if (strlen(extra))
    {
        tok = extra[0];
        if (tok == '.')
            sscanf(extra, ".%d", &d);
        else if (tok >= 'a' && tok <= 'z')
            d = tok;
    }
    return VERSION(a, b, c, d);
}
static void OnClientVersionChanged()
{
    g_clientVersion = ClientVersionFromString(uo_client_version.GetValue());
    ClientVersionFixup(g_clientVersion);
}
} // <anonymous> namespace

u32 GameVars::GetClientVersion()
{
    return g_clientVersion;
};

void GameVars::GetClientVersion(u32 *major, u32 *minor, u32 *rev, u32 *proto)
{
    if (major)
    {
        *major = (g_clientVersion >> 24) & 0xff;
    }

    if (minor)
    {
        *minor = (g_clientVersion >> 16) & 0xff;
    }

    if (rev)
    {
        *rev = (g_clientVersion >> 8) & 0xff;
    }

    if (proto)
    {
        *proto = (g_clientVersion & 0xff);
    }
}

Core::ConsoleVariable<std::string> uo_client_version("uo_client_version", "7.0.33.1", "UltimaOnline client version; Used to determine feature set.", Core::ConsoleFlags::None, OnClientVersionChanged, OnClientVersionChanged);
Core::ConsoleVariable<std::string> uo_server_address("uo_server_address", "");
Core::ConsoleVariable<u16> uo_server_port("uo_server_port", 0);
Core::ConsoleVariable<std::string> uo_login("uo_login", "", Core::ConsoleFlags::User);
Core::ConsoleVariable<std::string> uo_password("uo_password", "", Core::ConsoleFlags::User);
Core::ConsoleVariable<u8> uo_save_password("uo_save_password", 0, Core::ConsoleFlags::User);
Core::ConsoleVariable<u8> uo_auto_login("uo_auto_login", 0, Core::ConsoleFlags::User);
Core::ConsoleVariable<u8> uo_use_scaling("uo_use_scaling", 0, Core::ConsoleFlags::User);
Core::ConsoleVariable<u8> uo_render_border("uo_render_border", 2, Core::ConsoleFlags::User);
//Core::ConsoleVariable uo_client_flag("uo_client_flag", "", Core::ConsoleFlags::Persistent);
