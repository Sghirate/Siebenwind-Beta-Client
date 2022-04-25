#pragma once

#include "Core/Minimal.h"

namespace Core
{

struct Mutex
{
    Mutex();
    ~Mutex();

    void Lock();
    void Unlock();

private:
    void* m_handle = nullptr;
};

struct Thread
{
    Thread();
    virtual ~Thread();

    void Pause();
    void Resume();
    bool IsActive() const { return m_isActive; }
    bool IsPaused() const { return m_isPaused; }
    u64 GetThreadId() const { return m_threadId; }

protected:
    virtual void OnExecute(u32 nowTime) {}
    virtual void OnDestroy();

    Mutex m_mutex;

private:
    void* m_handle = nullptr;
    u64 m_threadId;
    bool m_isActive = false;
    bool m_isPaused = false;
};

};
