#pragma once

#include "Core/BaseTypes.h"

// Based on Mattias Gustavsson's PCG Implementation:
//  https://github.com/mattiasgustavsson/libs/blob/main/docs/rnd.md

namespace Core
{

struct Random
{
    static Random& Get();

    Random();
    Random(u32 a_seed);
    ~Random();

    void Seed(u32 a_seed);
    u32 GetNext();
    float GetNextF();
    u32 GetNextWrapped(i32 a_max);
    i32 GetNextInRange(i32 a_min, i32 a_max);

private:
    u64 m_state[2];
};

} // namespace Core
