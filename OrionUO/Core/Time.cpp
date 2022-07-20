#include "Time.h"
#include "Core/Minimal.h"
#include <chrono>
#include <thread>
#undef max
#undef min

namespace Core
{

typedef std::chrono::high_resolution_clock TClock;
typedef TClock::time_point TTimePoint;
typedef TClock::duration TDuration;
typedef std::chrono::duration<double> TSeconds;
typedef std::chrono::duration<double, std::milli> TMilliseconds;

static_assert(sizeof(TimeStamp::TData) >= sizeof(TTimePoint), "TimeStamp::TData is too small to store TTimePoint!");
static_assert(sizeof(TimeDiff::TData) >= sizeof(TDuration), "TimeDiff::TData is too small to store TDuration1");

TimeDiff::TimeDiff()
{
    new (&m_data)TDuration();
}

TimeDiff::TimeDiff(const TimeDiff& a_other)
{
    new (&m_data)TDuration(reinterpret_cast<const TDuration&>(a_other.m_data));
}

TimeDiff::~TimeDiff()
{
}

TimeDiff TimeDiff::FromSeconds(double a_seconds)
{
    TimeDiff result;
    reinterpret_cast<TDuration&>(result.m_data) = std::chrono::duration_cast<TDuration>(TSeconds(a_seconds));
    return result;
}

TimeDiff TimeDiff::FromMilliseconds(double a_milliseconds)
{
    TimeDiff result;
    reinterpret_cast<TDuration&>(result.m_data) = std::chrono::duration_cast<TDuration>(TMilliseconds(a_milliseconds));
    return result;
}

TimeDiff TimeDiff::FromDiff(const TimeStamp& a_lhs, const TimeStamp& a_rhs)
{
    const TTimePoint& lhs = reinterpret_cast<const TTimePoint&>(a_lhs.m_data);
    const TTimePoint& rhs = reinterpret_cast<const TTimePoint&>(a_rhs.m_data);

    TimeDiff result;
    reinterpret_cast<TDuration&>(result.m_data) = (lhs - rhs);
    return result;
}

double TimeDiff::GetSeconds() const
{
    return std::chrono::duration_cast<TSeconds>(reinterpret_cast<const TDuration&>(m_data)).count();
}

double TimeDiff::GetMilliseconds() const
{
    return std::chrono::duration_cast<TMilliseconds>(reinterpret_cast<const TDuration&>(m_data)).count();
}

TimeStamp::TimeStamp()
{
    new (&m_data)TTimePoint();
}

TimeStamp::TimeStamp(const TimeStamp& a_other)
{
    new (&m_data)TTimePoint(reinterpret_cast<const TTimePoint&>(a_other.m_data));
}

TimeStamp::~TimeStamp()
{
}

TimeStamp TimeStamp::Now()
{
    TimeStamp result;
    reinterpret_cast<TTimePoint&>(result.m_data) = TClock::now();
    return result;
}

TimeStamp TimeStamp::Max()
{
    TimeStamp result;
    reinterpret_cast<TTimePoint&>(result.m_data) = TTimePoint::max();
    return result;
}

TimeStamp TimeStamp::Min()
{
    TimeStamp result;
    reinterpret_cast<TTimePoint&>(result.m_data) = TTimePoint::min();
    return result;
}

void TimeStamp::Reset()
{
    TTimePoint unset;
    reinterpret_cast<TTimePoint&>(m_data) = unset;
}

bool TimeStamp::IsSet() const
{
    TTimePoint unset;
    return reinterpret_cast<const TTimePoint&>(m_data) != unset;
}

TimeStamp& TimeStamp::operator+=(const TimeDiff& a_other)
{
    TTimePoint& a = reinterpret_cast<TTimePoint&>(m_data);
    const TDuration b = reinterpret_cast<const TDuration&>(a_other.m_data);
    a += b;
    return *this;
}

TimeStamp& TimeStamp::operator-=(const TimeDiff& a_other)
{
    TTimePoint& a = reinterpret_cast<TTimePoint&>(m_data);
    const TDuration b = reinterpret_cast<const TDuration&>(a_other.m_data);
    a -= b;
    return *this;
}

TimeStamp& TimeStamp::operator=(const TimeStamp& a_other)
{
    m_data = a_other.m_data;
    return *this;
}

bool TimeStamp::operator<(const TimeStamp& a_other) const
{
    const TTimePoint& a = reinterpret_cast<const TTimePoint&>(m_data);
    const TTimePoint& b = reinterpret_cast<const TTimePoint&>(a_other.m_data);
    return a < b;
}

bool TimeStamp::operator<=(const TimeStamp& a_other) const
{
    const TTimePoint& a = reinterpret_cast<const TTimePoint&>(m_data);
    const TTimePoint& b = reinterpret_cast<const TTimePoint&>(a_other.m_data);
    return a <= b;
}

bool TimeStamp::operator>(const TimeStamp& a_other) const
{
    const TTimePoint& a = reinterpret_cast<const TTimePoint&>(m_data);
    const TTimePoint& b = reinterpret_cast<const TTimePoint&>(a_other.m_data);
    return a > b;
}

bool TimeStamp::operator>=(const TimeStamp& a_other) const
{
    const TTimePoint& a = reinterpret_cast<const TTimePoint&>(m_data);
    const TTimePoint& b = reinterpret_cast<const TTimePoint&>(a_other.m_data);
    return a >= b;
}

bool TimeStamp::operator==(const TimeStamp& a_other) const
{
    const TTimePoint& a = reinterpret_cast<const TTimePoint&>(m_data);
    const TTimePoint& b = reinterpret_cast<const TTimePoint&>(a_other.m_data);
    return a == b;
}

bool TimeStamp::operator!=(const TimeStamp& a_other) const
{
    const TTimePoint& a = reinterpret_cast<const TTimePoint&>(m_data);
    const TTimePoint& b = reinterpret_cast<const TTimePoint&>(a_other.m_data);
    return a != b;
}

TimeDiff operator-(const TimeStamp& a_lhs, const TimeStamp& a_rhs)
{
    return TimeDiff::FromDiff(a_lhs, a_rhs);
}

TimeStamp operator-(const TimeStamp& a_lhs, const TimeDiff& a_rhs)
{
    TimeStamp result = a_lhs;
    return result -= a_rhs;
}

TimeStamp operator+(const TimeStamp& a_lhs, const TimeDiff& a_rhs)
{
    TimeStamp result = a_lhs;
    return result += a_rhs;
}

Timer::Timer()
{
    Reset();
}

Timer::~Timer()
{
}

void Timer::Reset()
{
    m_start = TimeStamp::Now();
}

double Timer::GetElapsedSeconds() const
{
    return (TimeStamp::Now() - m_start).GetSeconds();
}

double Timer::GetElapsedMilliseconds() const
{
    return (TimeStamp::Now() - m_start).GetMilliseconds();
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
