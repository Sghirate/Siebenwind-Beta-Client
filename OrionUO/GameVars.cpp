#include "GameVars.h"
#include "plugin/enumlist.h"

namespace 
{
u32 g_clientVersion = CV_LATEST;
static u32 ClientVersionFromString(const char * a_str)
{
    if (!a_str || !a_str[0])
        return CV_LATEST;
    int a = 0, b = 0, c = 0, d = 0;
    char extra[16]{};
    char tok = 0;
    sscanf(a_str, "%d.%d.%d%s", &a, &b, &c, extra);
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
static void OnClientVersionInitialized()
{
    g_clientVersion = ClientVersionFromString(uo_client_version.GetString());
}
static void OnClientVersionChanged(const char* a_oldValue, const char* a_newValue)
{
    g_clientVersion = ClientVersionFromString(a_newValue);
}

} // <anonymous> namespace

u32 GameVars::GetClientVersion()
{
    return g_clientVersion;
};

Core::ConsoleVariable uo_login("uo_login", "", Core::ConsoleFlags::Persistent);
Core::ConsoleVariable uo_password("uo_password", "", Core::ConsoleFlags::Persistent);
Core::ConsoleVariable uo_client_version("uo_client_version", "7.0.33.1", "UltimaOnline client version; Used to determine feature set.", Core::ConsoleFlags::Persistent, OnClientVersionChanged, OnClientVersionInitialized);
Core::ConsoleVariable uo_game_dir("uo_game_dir", "", Core::ConsoleFlags::Persistent);
Core::ConsoleVariable uo_server_address("uo_server_address", "", Core::ConsoleFlags::Persistent);
Core::ConsoleVariable uo_server_port("uo_server_port", "", Core::ConsoleFlags::Persistent);
Core::ConsoleVariable uo_client_flag("uo_client_flag", "", Core::ConsoleFlags::Persistent);