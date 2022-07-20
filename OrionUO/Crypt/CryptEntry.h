#pragma once

#include "Core/Minimal.h"

namespace Crypt
{
void Init(bool a_isLogin, u8 a_seed[4]);
void Encrypt(bool a_isLogin, u8* a_src, u8* a_dest, int a_size);
void Decrypt(u8* a_src, u8* a_dest, int a_size);
size_t GetPluginsCount(); // FIXME: move this out

} // namespace Crypt
