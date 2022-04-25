#pragma once

namespace Core
{

struct Timer
{
    Timer();
    ~Timer();

    void Reset();
    double GetElapsedSeconds() const;
    double GetElapsedMilliseconds() const;

private:
    void* m_handle;
};

struct GameTimer : public Timer
{
    static GameTimer& Get();

private:
    GameTimer();
    ~GameTimer();

    using Timer::Reset;
};

struct FrameTimer : public Timer
{
    static FrameTimer& Get();

    void BeginFrame();
    void EndFrame();
    void SetTargetFPS(int a_targetFPS) { m_targetFPS = a_targetFPS; }
    int GetTargetFPS() const { return m_targetFPS; }
    double GetCurrentFrameSeconds(double a_max = 0.03333) const { return Core::Min(a_max, GetElapsedSeconds()); }
    double GetLastFrameSeconds(double a_max = 0.03333) const { return Core::Min(a_max, m_lastFrameSeconds); }
    double GetDeltaSeconds() const { return GetLastFrameSeconds(); }

private:
    FrameTimer();
    ~FrameTimer();

    using Timer::Reset;

    int m_targetFPS;
    double m_lastFrameSeconds;
};

} // namespace Core
