#include "Random.h"
#include "Core/Time.h"

namespace
{

static inline u64 GetTimeBasedSeed()
{
    Core::TimeStamp epoch;
    Core::TimeStamp now = Core::TimeStamp::Now();
    return (u64)(now - epoch).GetSeconds();
}
static inline u64 Murmur3Avalanche64(u64 a_value)
{
    a_value ^= a_value >> 33;
    a_value *= 0xff51afd7ed558ccd;
    a_value ^= a_value >> 33;
    a_value *= 0xc4ceb9fe1a85ec53;
    a_value ^= a_value >> 33;
    return a_value;
}
static inline float GetFloatFromU32(u32 value)
{
    u32 exponent  = 127;
    u32 mantissa  = value >> 9;
    u32 result    = (exponent << 23) | mantissa;
    float fresult = *(float*)(&result);
    return fresult - 1.0f;
}

} // namespace

namespace Core
{

Random& Random::Get()
{
    Random s_instance;
    return s_instance;
}

Random::Random()
{
    Seed(GetTimeBasedSeed());
}

Random::Random(u32 a_seed)
{
    Seed(a_seed);
}

Random::~Random()
{
}

void Random::Seed(u32 a_seed)
{
    u64 value  = (((u64)a_seed) << 1ULL) | 1ULL;
    value      = Murmur3Avalanche64(value);
    m_state[0] = 0U;
    m_state[1] = (value << 1ULL) | 1ULL;
    GetNext();
    m_state[0] += Murmur3Avalanche64(value);
    GetNext();
}

u32 Random::GetNext()
{
    u64 oldstate   = m_state[0];
    m_state[0]     = oldstate * 0x5851f42d4c957f2dULL + m_state[1];
    u32 xorshifted = (u32)(((oldstate >> 18ULL) ^ oldstate) >> 27ULL);
    u32 rot        = (u32)(oldstate >> 59ULL);
    return (xorshifted >> rot) | (xorshifted << ((-(int)rot) & 31));
}

float Random::GetNextF()
{
    return GetFloatFromU32(GetNext());
}

u32 Random::GetNextWrapped(i32 a_max)
{
    return GetNext() % a_max;
}

i32 Random::GetNextInRange(i32 a_min, i32 a_max)
{
    const i64 range = ((i64)a_max - (i64)a_min) + 1;
    if (range <= 0)
        return a_min;
    const i64 value = (i64)((double)GetNextF() * (double)range);
    return (i32)(a_min + value);
}

} // namespace Core
