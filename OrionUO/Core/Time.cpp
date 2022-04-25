#include "Time.h"
#include "Core/Minimal.h"
#include <chrono>
#include <thread>

namespace Core
{
typedef std::chrono::high_resolution_clock TClock;
typedef TClock::time_point TTimePoint;
typedef std::chrono::duration<double> TSeconds;
typedef std::chrono::duration<double, std::milli> TMilliseconds;

Timer::Timer()
    : m_handle(new TTimePoint())
{
    Reset();
}

Timer::~Timer()
{
    if (m_handle)
    {
        delete (m_handle);
        m_handle = nullptr;
    }
}

void Timer::Reset()
{
    if (m_handle)
        *static_cast<TTimePoint*>(m_handle) = TClock::now();
}

double Timer::GetElapsedSeconds() const
{
    return m_handle ? TSeconds(TClock::now() - *static_cast<TTimePoint*>(m_handle)).count() : 0.0;
}

double Timer::GetElapsedMilliseconds() const
{
    return m_handle ? TMilliseconds(TClock::now() - *static_cast<TTimePoint*>(m_handle)).count() :
                      0.0;
}

GameTimer& GameTimer::Get()
{
    static GameTimer s_instance;
    return s_instance;
}

GameTimer::GameTimer()
{
}

GameTimer::~GameTimer()
{
}

FrameTimer& FrameTimer::Get()
{
    static FrameTimer s_instance;
    return s_instance;
}

FrameTimer::FrameTimer()
    : m_targetFPS(0)
    , m_lastFrameSeconds(0.0)
{
}

FrameTimer::~FrameTimer()
{
}

void FrameTimer::BeginFrame()
{
    Reset();
}

void FrameTimer::EndFrame()
{
    if (m_targetFPS > 0)
    {
        const double elapsedMs = GetElapsedMilliseconds();
        const double targetMs  = 1000.0 / static_cast<double>(m_targetFPS);
        if (elapsedMs < targetMs)
        {
            TMilliseconds deltaMs(targetMs - elapsedMs);
            auto sleepDuration = std::chrono::duration_cast<std::chrono::milliseconds>(deltaMs);
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepDuration.count()));
        }
    }
    m_lastFrameSeconds = GetElapsedSeconds();
}

} // namespace Core
