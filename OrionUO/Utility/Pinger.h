#pragma once

#include "Core/Console.h"
#include "Core/Minimal.h"
#include <string>

extern Core::ConsoleVariable<u8> ping_disable;

struct PingResult
{
    u32 serverId = 0;
    int min = 9999;
    int max = 0;
    int avg = 0;
    int lost = 0;
};
typedef void(*PingCallback)(const PingResult&);
struct Pinger
{
    static Pinger& Get();

    void Ping(u32 a_serverId, const std::string& a_serverAddress, int a_count, PingCallback a_callback);

private:
    Pinger();
    ~Pinger();
};
