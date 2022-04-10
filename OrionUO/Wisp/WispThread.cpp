// MIT License

#include <thread>
#include <SDL_timer.h>
#include <SDL_thread.h>
#define THREAD_USE_CLOCK 0

#if USE_WISP
#define THREADCALL __stdcall
typedef unsigned thread_int;
#else
#define THREADCALL SDLCALL
typedef int thread_int;
#endif

namespace Wisp
{
thread_int THREADCALL CThreadLoop(void *arg)
{
    CThread *parent = (CThread *)arg;

    while (parent->IsActive())
    {
        while (parent->Paused())
        {
            SDL_Delay(1);
        }

#if THREAD_USE_CLOCK == 1
        parent->OnExecute(clock());
#else
        parent->OnExecute(SDL_GetTicks());
#endif

        if (!parent->Cycled())
        {
            break;
        }

        int delay = parent->Delay();

        if (delay > 0)
        {
            SDL_Delay(delay);
        }
    }

    parent->OnDestroy();
    delete parent;

#if USE_WISP
    _endthreadex(0);
#endif

    return 0;
};

thread_int THREADCALL CThreadLoopSynchronizedDelay(void *arg)
{
    CThread *parent = (CThread *)arg;

    while (parent->IsActive())
    {
        while (parent->Paused())
        {
            SDL_Delay(1);
        }

#if THREAD_USE_CLOCK == 1
        u32 nowTime = clock();
#else
        u32 nowTime = SDL_GetTicks();
#endif

        parent->OnExecute(nowTime);
        if (!parent->Cycled())
        {
            break;
        }

#if THREAD_USE_CLOCK == 1
        int delay = (int)((nowTime + parent->Delay()) - clock());
#else
        int delay = (int)((nowTime + parent->Delay()) - SDL_GetTicks());
#endif

        SDL_Delay(delay > 0 ? delay : 1);
    }

    parent->OnDestroy();
    delete parent;

#if USE_WISP
    _endthreadex(0);
#endif

    return 0;
};

CThread::CThread()
{
    CREATE_MUTEX(m_Mutex);
}

CThread::~CThread()
{
}

void CThread::OnDestroy()
{
    RELEASE_MUTEX(m_Mutex);
#if USE_WISP
    if (m_Handle != 0)
        ::CloseHandle(m_Handle);
    m_Handle = 0;
#else
    if (m_Handle != nullptr)
    {
        SDL_DetachThread(m_Handle);
    }
    m_Handle = nullptr;
#endif
    m_ID = 0;
}

void CThread::Run(bool cycled, int delay, bool synchronizedDelay)
{
    if (!m_Active && m_Handle == 0)
    {
        m_Cycled = cycled;
        m_Delay = delay;
        m_Active = true;

#if USE_WISP
        if (synchronizedDelay)
            m_Handle =
                (HANDLE)_beginthreadex(nullptr, 0, CThreadLoopSynchronizedDelay, this, 0, &m_ID);
        else
            m_Handle = (HANDLE)_beginthreadex(nullptr, 0, CThreadLoop, this, 0, &m_ID);
#else
        if (synchronizedDelay)
        {
            m_Handle = SDL_CreateThread(
                CThreadLoopSynchronizedDelay, "CThreadLoopSynchronizedDelay", (void *)this);
        }
        else
        {
            m_Handle = SDL_CreateThread(CThreadLoop, "CThreadLoop", (void *)this);
        }
#endif
    }
}

bool CThread::IsActive()
{
    LOCK(m_Mutex);
    bool result = m_Active;
    UNLOCK(m_Mutex);
    return result;
}

void CThread::Stop()
{
    LOCK(m_Mutex);
    m_Active = false;
    UNLOCK(m_Mutex);
}

bool CThread::Cycled()
{
    LOCK(m_Mutex);
    bool result = m_Cycled;
    UNLOCK(m_Mutex);
    return result;
}

void CThread::Pause()
{
    LOCK(m_Mutex);
    m_Paused = true;
    UNLOCK(m_Mutex);
}

void CThread::Resume()
{
    LOCK(m_Mutex);
    m_Paused = false;
    UNLOCK(m_Mutex);
}

bool CThread::Paused()
{
    LOCK(m_Mutex);
    bool result = m_Paused;
    UNLOCK(m_Mutex);
    return result;
}

int CThread::Delay()
{
    LOCK(m_Mutex);
    int result = m_Delay;
    UNLOCK(m_Mutex);
    return result;
}

void CThread::ChangeDelay(int newDelay)
{
    LOCK(m_Mutex);
    m_Delay = newDelay;
    UNLOCK(m_Mutex);
}

SDL_threadID CThread::GetCurrentThreadId()
{
    return SDL_GetThreadID(nullptr);
}

}; // namespace Wisp
