#pragma once

#include "Core/Minimal.h"
#include <string>

struct PING_INFO_DATA
{
    u32 ServerID = 0;
    int Min = 9999;
    int Max = 0;
    int Average = 0;
    int Lost = 0;
};

class CPingThread : public Wisp::CThread
{
    u32 ServerID = 0xFFFFFFFF;
    std::string ServerIP = "";
    int RequestsCount = 10;

private:
    int CalculatePing();

public:
    CPingThread(int serverID, const std::string &serverIP, int requestsCount);
    virtual ~CPingThread();

    virtual void OnExecute(u32 nowTime) override;
    static const u32 MessageID = USER_MESSAGE_ID + 401;
};
