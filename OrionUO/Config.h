// GPLv3 License
// Copyright (C) 2019 Danny Angelo Carminati Grein

#pragma once

struct Config
{
    std::string Login;
    std::string Password;
    std::string ClientVersionString = "7.0.33.1";
    std::string CustomPath;
    std::string ServerAddress;
    u16 ServerPort = 2593;
    u16 ClientFlag = 0;
    bool SavePassword = false;
    bool AutoLogin = false;
    bool TheAbyss = false;
    bool Asmut = false;
    bool UseVerdata = false;
    bool UseCrypt = false;

    // Calculated stuff that are not saved back
    u16 Seed = 0x1357;
    u32 Key1 = 0;
    u32 Key2 = 0;
    u32 Key3 = 0;
    u32 ClientVersion = CV_LATEST;
    u32 EncryptionType = 0;
};

void GetClientVersion(u32 *major, u32 *minor, u32 *rev, u32 *proto = nullptr);
void LoadGlobalConfig();
void SaveGlobalConfig();

extern Config g_Config;
