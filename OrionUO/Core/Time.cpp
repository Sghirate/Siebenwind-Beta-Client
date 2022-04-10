#include "Time.h"
#include "Core/Minimal.h"
#include <chrono>
#include <thread>

namespace Core
{

static struct CoreClock
{
    typedef std::chrono::high_resolution_clock TClock;
    typedef TClock::time_point TTimePoint;
    typedef std::chrono::duration<double> TSeconds;
    typedef std::chrono::duration<double, std::milli> TMilliseconds;

    TTimePoint gameStart  = TClock::now();
    TTimePoint frameStart = TClock::now();
    TSeconds lastFrameDuration;
    double deltaSeconds = 0;
    int targetFps = 0;

    void Reset()
    {
        gameStart = frameStart = TClock::now();
        lastFrameDuration      = TSeconds::zero();
        targetFps              = 0;
    }
    TTimePoint Now() const { return TClock::now(); }
} g_coreClock;

void Time::Init()
{
    g_coreClock.Reset();
}

void Time::BeginFrame()
{
    g_coreClock.frameStart = g_coreClock.Now();
}

void Time::EndFrame()
{
    if (g_coreClock.targetFps > 0)
    {
        const double targetMs       = 1000.0 / static_cast<double>(g_coreClock.targetFps);
        CoreClock::TTimePoint end   = g_coreClock.Now();
        auto frameEnd               = g_coreClock.Now();
        CoreClock::TMilliseconds ms = end - g_coreClock.frameStart;
        if (ms.count() < targetMs)
        {
            CoreClock::TMilliseconds deltaMs(targetMs - ms.count());
            auto sleepDuration = std::chrono::duration_cast<std::chrono::milliseconds>(deltaMs);
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepDuration.count()));
        }
    }
    g_coreClock.lastFrameDuration = CoreClock::TSeconds(g_coreClock.Now() - g_coreClock.frameStart);
    g_coreClock.deltaSeconds      = GetLastFrameSeconds();
}

void Time::Shutdown()
{
}

double Time::GetDeltaSeconds()
{
    return g_coreClock.deltaSeconds;
}

double Time::GetCurrentFrameSeconds(double a_max /* = 0.03333*/)
{
    CoreClock::TSeconds cur(g_coreClock.Now() - g_coreClock.frameStart);
    return Core::Min(a_max, cur.count());
}

double Time::GetLastFrameSeconds(double a_max /* = 0.03333*/)
{
    return Core::Min(a_max, g_coreClock.lastFrameDuration.count());
}

double Time::GetTotalSeconds()
{
    CoreClock::TSeconds cur(g_coreClock.Now() - g_coreClock.gameStart);
    return cur.count();
}

int Time::GetTargetFps()
{
    return g_coreClock.targetFps;
}

void Time::SetTargetFps(int a_targetFps)
{
    g_coreClock.targetFps = a_targetFps;
}

} // namespace Core
