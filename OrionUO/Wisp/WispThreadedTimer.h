// MIT License

#ifndef WISP_TT_H
#define WISP_TT_H

#if USE_TIMERTHREAD

#pragma once

namespace Wisp
{
class CThreadedTimer : public Wisp::CThread
{
public:
    static const u32 MessageID = USER_MESSAGE_ID + 400;
    WindowHandle m_Handle = 0;
    u32 TimerID = 0;
    bool WaitForProcessMessage = false;

    CThreadedTimer(u32 id, WindowHandle handle, bool waitForProcessMessage = false);
    virtual ~CThreadedTimer();

    virtual void OnExecute(u32 nowTime);
    virtual void OnDestroy();
};

}; // namespace Wisp

#endif // USE_TIMERTHREAD

#endif // WISP_TT_H
