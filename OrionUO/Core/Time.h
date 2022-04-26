#pragma once

#include "Core/Minimal.h"

namespace Core
{

struct TimeStamp
{
    typedef u64 TData;

    TimeStamp();
    TimeStamp(const TimeStamp& a_other);
    ~TimeStamp();

    static TimeStamp Now();

    void Reset();
    bool IsSet() const;

    TimeStamp& operator+=(const struct TimeDiff& a_other);
    TimeStamp& operator-=(const struct TimeDiff& a_other);
    TimeStamp& operator=(const TimeStamp& a_other);
    bool operator<(const TimeStamp& a_other) const;
    bool operator<=(const TimeStamp& a_other) const;
    bool operator>(const TimeStamp& a_other) const;
    bool operator>=(const TimeStamp& a_other) const;
    bool operator==(const TimeStamp& a_other) const;
    bool operator!=(const TimeStamp& a_other) const;

private:
    friend struct TimeDiff;
    u64 m_data;
};
struct TimeDiff
{
    typedef u64 TData;

    TimeDiff();
    TimeDiff(const TimeDiff& a_other);
    ~TimeDiff();

    static TimeDiff FromSeconds(double a_seconds);
    static TimeDiff FromMilliseconds(double a_milliseconds);
    static TimeDiff FromDiff(const TimeStamp& a_lhs, const TimeStamp& a_rhs);

    double GetSeconds() const;
    double GetMilliseconds() const;

private:
    friend struct TimeStamp;
    u64 m_data;
};
TimeDiff operator-(const TimeStamp& a_lhs, const TimeStamp& a_rhs);
TimeStamp operator-(const TimeStamp& a_lhs, const TimeDiff& a_rhs);
TimeStamp operator+(const TimeStamp& a_lhs, const TimeDiff& a_rhs);

struct Timer
{
    Timer();
    ~Timer();

    void Reset();
    double GetElapsedSeconds() const;
    double GetElapsedMilliseconds() const;
    inline const TimeStamp& GetStart() const { return m_start; }

private:
    TimeStamp m_start;
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
    static const TimeStamp& Now() { return Get().GetStart(); }

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

namespace TimeLiterals
{

inline TimeDiff operator ""_s(long double a_seconds) { return TimeDiff::FromSeconds(a_seconds); }
inline TimeDiff operator ""_ms(long double a_seconds) { return TimeDiff::FromMilliseconds(a_seconds); }
inline TimeDiff operator ""_s(unsigned long long int a_seconds) { return TimeDiff::FromSeconds((double)a_seconds); }
inline TimeDiff operator ""_ms(unsigned long long int a_seconds) { return TimeDiff::FromMilliseconds((double)a_seconds); }

} // namespace TimeLiterals

} // namespace Core
