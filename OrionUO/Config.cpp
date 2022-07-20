#include "Config.h"

#include "Core/File.h"
#include "Core/StringUtils.h"
#include "Core/TextFileParser.h"
#include "Globals.h"
#include "Definitions.h"
#include "OrionApplication.h"
#include "plugin/enumlist.h"
#include "Crypt/CryptEntry.h"
#include "Managers/PacketManager.h"
#include <string>

#define ORIONUO_CONFIG "OrionUO.cfg"

Config g_Config;

enum
{
    MSCC_NONE,
    MSCC_ACTID,
    MSCC_ACTPWD,
    MSCC_REMEMBERPWD,
    MSCC_AUTOLOGIN,
    MSCC_THE_ABYSS,
    MSCC_ASMUT,
    MSCC_CUSTOM_PATH,
    MSCC_LOGIN_SERVER,
    MSCC_CLIENT_VERSION,
    MSCC_USE_CRYPT,
    MSCC_USE_VERDATA,
    MSCC_CLIENT_TYPE,
    MSCC_COUNT,
};

namespace config
{
struct Modified
{
    bool UseVerdata = false;
    bool ClientFlag = false;
};

struct ConfigEntry
{
    u32 key;
    const char *key_name;
};

static const ConfigEntry s_Keys[] = {
    { MSCC_ACTID, "acctid" },
    { MSCC_ACTPWD, "acctpassword" },
    { MSCC_REMEMBERPWD, "rememberacctpw" },
    { MSCC_AUTOLOGIN, "autologin" },
    { MSCC_THE_ABYSS, "theabyss" },
    { MSCC_ASMUT, "asmut" },
    { MSCC_CUSTOM_PATH, "custompath" },
    { MSCC_LOGIN_SERVER, "loginserver" },
    { MSCC_CLIENT_VERSION, "clientversion" },
    { MSCC_USE_CRYPT, "crypt" },
    { MSCC_USE_VERDATA, "useverdata" },
    { MSCC_CLIENT_TYPE, "clienttype" },
    { MSCC_COUNT, nullptr },
};

static u32 GetConfigKey(const std::string &key)
{
    auto str = Core::ToLowerA(key);
    for (int i = 0; s_Keys[i].key_name; i++)
    {
        if (str == s_Keys[i].key_name)
        {
            return s_Keys[i].key;
        }
    }
    return MSCC_NONE;
}

} // namespace config

static config::Modified s_Mark;

static CLIENT_FLAG GetClientTypeFromString(const std::string &str)
{
    auto client = Core::ToLowerA(str);
    if (client == "t2a")
    {
        return CF_T2A;
    }
    if (client == "re")
    {
        return CF_RE;
    }
    if (client == "td")
    {
        return CF_TD;
    }
    if (client == "lbr")
    {
        return CF_LBR;
    }
    if (client == "aos")
    {
        return CF_AOS;
    }
    if (client == "se")
    {
        return CF_SE;
    }
    if (client == "sa")
    {
        return CF_SA;
    }

    return CF_UNDEFINED;
}

static const char *GetClientTypeString(u16 clientFlag)
{
    switch (clientFlag)
    {
        case CF_T2A:
            return "t2a";
        case CF_RE:
            return "re";
        case CF_TD:
            return "td";
        case CF_LBR:
            return "lbr";
        case CF_AOS:
            return "aos";
        case CF_SE:
            return "se";
        case CF_SA:
            return "sa";
        default:
            return "";
    }
    return "";
}

// Reference: https://github.com/polserver/polserver/blob/5c747bb88123945bb892d3d793b89afcb1dc645a/pol-core/pol/crypt/cryptkey.cpp
static void SetClientCrypt(u32 version)
{
    if (version == CV_200X)
    {
        g_Config.Key1 = 0x2D13A5FC;
        g_Config.Key2 = 0x2D13A5FD;
        g_Config.Key3 = 0xA39D527F;
        g_Config.EncryptionType = ET_203;
        return;
    }

    int a = (version >> 24) & 0xff;
    int b = (version >> 16) & 0xff;
    int c = (version >> 8) & 0xff;

    int temp = ((a << 9 | b) << 10 | c) ^ ((c * c) << 5);
    g_Config.Key2 = (temp << 4) ^ (b * b) ^ (b * 0x0B000000) ^ (c * 0x380000) ^ 0x2C13A5FD;
    temp = (((a << 9 | c) << 10 | b) * 8) ^ (c * c * 0x0c00);
    g_Config.Key3 = temp ^ (b * b) ^ (b * 0x6800000) ^ (c * 0x1c0000) ^ 0x0A31D527F;

    // Configurator does this, not sure why
    // lets keep compatibility until we understand more
    g_Config.Key1 = g_Config.Key2 - 1;

    if (version < VERSION(1, 25, 35, 0))
    {
        g_Config.EncryptionType = ET_OLD_BFISH;
    }
    else if (version == VERSION(1, 25, 36, 0))
    {
        g_Config.EncryptionType = ET_1_25_36;
    }
    else if (version < CV_200)
    {
        g_Config.EncryptionType = ET_BFISH;
    }
    else if (version <= VERSION(2, 0, 3, 0))
    {
        g_Config.EncryptionType = ET_203;
    }
    else
    {
        g_Config.EncryptionType = ET_TFISH;
    }
}

static CLIENT_FLAG GetClientType(uint32_t version)
{
    if (version < CV_200)
    {
        return CF_T2A;
    }
    if (version < CV_300)
    {
        return CF_RE;
    }
    if (version < CV_308)
    {
        return CF_TD;
    }
    if (version < CV_308Z)
    {
        return CF_LBR;
    }
    if (version < CV_405A)
    {
        // >=3.0.8z
        return CF_AOS;
    }
    if (version < CV_60144)
    {
        return CF_SE;
    }
    return CF_SA;
}

void ClientVersionFixup(u32 a_version)
{
    //SetClientVersion(versionStr);
    SetClientCrypt(a_version);

    const bool useVerdata = a_version < CV_500A;
    const auto clientType = GetClientType(a_version);

    if (a_version < CV_500A)
    {
        g_MapSize[0].x = 6144;
        g_MapSize[1].x = 6144;
    }

    if (!g_Config.UseCrypt)
    {
        g_Config.EncryptionType = ET_NOCRYPT;
    }

    if (a_version >= CV_70331)
    {
        g_MaxViewRange = MAX_VIEW_RANGE_NEW;
    }
    else
    {
        g_MaxViewRange = MAX_VIEW_RANGE_OLD;
    }

    g_PacketManager.ConfigureClientVersion(a_version);

    if (!s_Mark.ClientFlag)
    {
        g_Config.ClientFlag = clientType;
    }
    if (!s_Mark.UseVerdata)
    {
        g_Config.UseVerdata = useVerdata;
    }
}

void LoadGlobalConfig()
{

//    LOG("Loading global config from " ORIONUO_CONFIG "\n");

    Core::TextFileParser file(g_App.GetExeDir() / ORIONUO_CONFIG, "=,", "#;", "");
    while (!file.IsEOF())
    {
        auto strings = file.ReadTokens(false); // Trim remove spaces from paths
        if (strings.size() >= 2)
        {
            const auto key = config::GetConfigKey(strings[0]);
            switch (key)
            {
                break;
                case MSCC_CUSTOM_PATH:
                {
                    g_App.SetGameDir(strings[1]);
                }
                break;
                case MSCC_ACTID:
                {
                    g_Config.Login = strings[1];
                    break;
                }
                case MSCC_ACTPWD:
                {
                    const std::string& password = file.GetRawLine();
                    size_t pos = password.find_first_of('=');
                    g_Config.Password = password.substr(pos + 1, password.length() - (pos + 1));
                    break;
                }
                case MSCC_REMEMBERPWD:
                {
                    g_Config.SavePassword = Core::ToBool(strings[1]);
                    break;
                }
                case MSCC_AUTOLOGIN:
                {
                    g_Config.AutoLogin = Core::ToBool(strings[1]);
                    break;
                }
                case MSCC_THE_ABYSS:
                {
                    g_Config.TheAbyss = Core::ToBool(strings[1]);
                    break;
                }
                case MSCC_ASMUT:
                {
                    g_Config.Asmut = Core::ToBool(strings[1]);
                    break;
                }
                case MSCC_LOGIN_SERVER:
                {
                    g_Config.ServerAddress = strings[1];
                    g_Config.ServerPort = Core::ToInt(strings[2]);
                    break;
                }
                case MSCC_USE_CRYPT:
                {
                    g_Config.UseCrypt = Core::ToBool(strings[1]);
                    break;
                }
                case MSCC_USE_VERDATA:
                {
                    s_Mark.UseVerdata = true;
                    g_Config.UseVerdata = Core::ToBool(strings[1]);
                    break;
                }
                case MSCC_CLIENT_TYPE:
                {
                    auto type = GetClientTypeFromString(strings[1]);
                    if (type != CF_UNDEFINED)
                    {
                        s_Mark.ClientFlag = true;
                        g_Config.ClientFlag = type;
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }
}

void SaveGlobalConfig()
{
//    LOG("Saving global config to " ORIONUO_CONFIG "\n");
    Core::File cfg(g_App.GetExeDir() / ORIONUO_CONFIG, "w");
    if (!cfg)
    {
        return;
    }

    cfg.Print("AcctID=%s\n", g_Config.Login.c_str());
    if (g_Config.SavePassword)
    {
        cfg.Print("AcctPassword=%s\n", g_Config.Password.c_str());
        cfg.Print("RememberAcctPW=yes\n");
    }
    else
    {
        cfg.Print("AcctPassword=\n");
        cfg.Print("RememberAcctPW=no\n");
    }

    cfg.Print("AutoLogin=%s\n", (g_Config.AutoLogin ? "yes" : "no"));
    cfg.Print("TheAbyss=%s\n", (g_Config.TheAbyss ? "yes" : "no"));
    cfg.Print("Asmut=%s\n", (g_Config.Asmut ? "yes" : "no"));

    if (s_Mark.ClientFlag)
    {
        cfg.Print("ClientType=%s\n", GetClientTypeString(g_Config.ClientFlag));
    }
    if (s_Mark.UseVerdata)
    {
        cfg.Print("UseVerdata=%s\n", (g_Config.UseVerdata ? "yes" : "no"));
    }

    cfg.Print("Crypt=%s\n", (g_Config.UseCrypt ? "yes" : "no"));
    if (g_App.GetGameDir() != g_App.GetExeDir())
    {
        cfg.Print("CustomPath=%s\n", g_App.GetGameDir().string().c_str());
    }

    if (!g_Config.ServerAddress.empty())
    {
        cfg.Print("LoginServer=%s,%d\n", g_Config.ServerAddress.c_str(), g_Config.ServerPort);
    }

    cfg.Flush();
}
