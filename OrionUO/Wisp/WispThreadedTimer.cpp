// MIT License

#if USE_TIMERTHREAD

#include <SDL_events.h>

namespace Wisp
{
CThreadedTimer::CThreadedTimer(u32 id, WindowHandle handle, bool waitForProcessMessage)
    : TimerID(id)
    , m_Handle(handle)
    , WaitForProcessMessage(waitForProcessMessage)
{
}

CThreadedTimer::~CThreadedTimer()
{
}

void CThreadedTimer::OnExecute(u32 nowTime)
{
    if (IsActive())
    {
#if USE_WISP
        if (WaitForProcessMessage)
            SendMessage(m_Handle, MessageID, nowTime, (LPARAM)this);
        else
            PostMessage(m_Handle, MessageID, nowTime, (LPARAM)this);
#else
        PUSH_EVENT(MessageID, nowTime, this);
#endif
    }
}

void CThreadedTimer::OnDestroy()
{
    if (Wisp::g_WispWindow != nullptr)
    {
        Wisp::g_WispWindow->RemoveThreadedTimer(TimerID);
    }
}

}; // namespace Wisp

#endif // USE_TIMERTHREAD
