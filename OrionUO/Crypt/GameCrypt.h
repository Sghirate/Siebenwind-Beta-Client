#pragma once

struct Blowfish
{
    static void Init();
    static void Encrypt(u8* a_in, u8* a_out, int a_len);
};
struct Twofish
{
    static void Init(u8 seed[4]);
    static void InitMD5();
    static void Encrypt(const u8* a_in, u8* a_out, int a_size);
    static void Decrypt(const u8* a_in, u8* a_out, int a_size);
};
