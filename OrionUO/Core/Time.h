#pragma once

namespace Core
{

struct Time
{
    static void Init();
    static void BeginFrame();
    static void EndFrame();
    static void Shutdown();

    static double GetDeltaSeconds();
    static double GetCurrentFrameSeconds(double a_max = 0.03333);
    static double GetLastFrameSeconds(double a_max = 0.03333);
    static double GetTotalSeconds();
    static int GetTargetFps();
    static void SetTargetFps(int a_targetFps);
};

} // namespace Core
