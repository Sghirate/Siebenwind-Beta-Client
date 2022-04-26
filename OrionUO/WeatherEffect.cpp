#include "WeatherEffect.h"
#include "Core/Random.h"

CWeatherEffect::CWeatherEffect()
{
    ID = Core::Random::Get().GetNextWrapped(2000);

    ScaleRatio = (float)(ID % 20) / 13.0f;
}
