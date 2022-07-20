
#include "CryptEntry.h"
#include "plugin/enumlist.h"
#include "Config.h"
#include "LoginCrypt.h"
#include "GameCrypt.h"
#include <cstring>
#include <memory>

static size_t s_CryptPluginsCount = 0;

namespace Crypt
{
void Init(bool a_isLogin, u8 a_seed[4])
{
    if (a_isLogin)
    {
        g_LoginCrypt.Init(a_seed);
    }
    else
    {
        if (g_Config.EncryptionType != ET_NOCRYPT)
            Blowfish::Init();

        if (g_Config.EncryptionType == ET_203 || g_Config.EncryptionType == ET_TFISH)
        {
            Twofish::Init(a_seed);
            if (g_Config.EncryptionType == ET_TFISH)
                Twofish::InitMD5();
        }
    }
}

void Encrypt(bool a_isLogin, u8* a_src, u8* a_dest, int a_size)
{
    if (g_Config.EncryptionType == ET_NOCRYPT)
    {
        memcpy(a_dest, a_src, a_size);
    }
    else if (a_isLogin)
    {
        if (g_Config.EncryptionType == ET_OLD_BFISH)
            g_LoginCrypt.Encrypt_Old(a_src, a_dest, a_size);
        else if (g_Config.EncryptionType == ET_1_25_36)
            g_LoginCrypt.Encrypt_1_25_36(a_src, a_dest, a_size);
        else if (g_Config.EncryptionType != ET_NOCRYPT)
            g_LoginCrypt.Encrypt(a_src, a_dest, a_size);
    }
    else if (g_Config.EncryptionType == ET_203)
    {
        Blowfish::Encrypt(a_src, a_dest, a_size);
        Twofish::Encrypt(a_dest, a_dest, a_size);
    }
    else if (g_Config.EncryptionType == ET_TFISH)
    {
        Twofish::Encrypt(a_src, a_dest, a_size);
    }
    else
    {
        Blowfish::Encrypt(a_src, a_dest, a_size);
    }
}

void Decrypt(u8* a_src, u8* a_dest, int a_size)
{
    if (g_Config.EncryptionType == ET_TFISH)
        Twofish::Decrypt(a_src, a_dest, a_size);
    else
        memcpy(a_dest, a_src, a_size);
}

size_t GetPluginsCount()
{
    return s_CryptPluginsCount;
}

} // namespace Crypt
