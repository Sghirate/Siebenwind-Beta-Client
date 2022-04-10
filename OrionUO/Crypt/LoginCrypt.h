// MIT License

#pragma once

class CLoginCrypt
{
private:
    u32 m_k1;
    u32 m_k2;
    u32 m_k3;
    u32 m_key[2];
    u8 m_seed[4];

public:
    CLoginCrypt();
    ~CLoginCrypt() {}

    void Init(u8 ps[4]);
    void Encrypt(const u8 *in, u8 *out, int size);
    void Encrypt_Old(const u8 *in, u8 *out, int size);
    void Encrypt_1_25_36(const u8 *in, u8 *out, int size);
};

extern CLoginCrypt g_LoginCrypt;
