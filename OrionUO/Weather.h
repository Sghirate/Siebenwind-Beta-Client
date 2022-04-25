#pragma once

#include "WeatherEffect.h"

class CWeather
{
public:
    u8 Type = 0;
    u8 Count = 0;
    u8 CurrentCount = 0;
    u8 Temperature = 0;
    u32 Timer = 0;
    char Wind = 0;
    u32 WindTimer = 0;
    u32 LastTick = 0;
    float SimulationRatio = 37.0f;

    CWeather();
    ~CWeather() { m_Effects.clear(); }

    std::deque<CWeatherEffect> m_Effects;

    void Reset();
    void Generate();
    void Draw(int x, int y);
};

extern CWeather g_Weather;
