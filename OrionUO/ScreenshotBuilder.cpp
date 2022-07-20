#include "ScreenshotBuilder.h"
#include "GameWindow.h"
#include "OrionApplication.h"
#include "Globals.h"
#include "OrionUO.h"
#include "Profiler.h"
#include "Managers/ConfigManager.h"
#include "plugin/enumlist.h"
#include <time.h>

#define STBIWDEF static inline
#define STB_IMAGE_WRITE_STATIC
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

CScreenshotBuilder g_ScreenshotBuilder;

static std::vector<u32> GetScenePixels(int x, int y, int width, int height)
{
    std::vector<u32> pixels(width * height);

    glReadPixels(
        x,
        g_gameWindow.GetSize().y - y - height,
        width,
        height,
        GL_RGBA,
        GL_UNSIGNED_INT_8_8_8_8_REV,
        &pixels[0]);

    for (u32 &i : pixels)
        i |= 0xFF000000;

    return pixels;
}

CScreenshotBuilder::CScreenshotBuilder()
{
}

CScreenshotBuilder::~CScreenshotBuilder()
{
}

void CScreenshotBuilder::SaveScreen()
{
    SaveScreen(0, 0, g_gameWindow.GetSize().x, g_gameWindow.GetSize().y);
}

void CScreenshotBuilder::SaveScreen(int a_x, int a_y, int a_w, int a_h)
{
    PROFILER_EVENT();
    std::filesystem::path path = g_App.GetExeDir() / "screenshots";
    std::filesystem::create_directory(path);

    auto t = time(nullptr);
    auto now = *localtime(&t);
    char buf[100]{};
    sprintf_s(
        buf,
        "/%d%d%d_%d%d%d",
        now.tm_year + 1900,
        now.tm_mon,
        now.tm_mday,
        now.tm_hour,
        now.tm_min,
        now.tm_sec);
    path /= buf;

    auto pixels = GetScenePixels(a_x, a_y, a_w, a_h);
    int result = 0;
    auto data = (void *)&pixels[0];
    stbi_flip_vertically_on_write(true);
    switch (g_ConfigManager.ScreenshotFormat)
    {
        case SF_PNG:
        {
            path.replace_extension(".png");
            result = stbi_write_png(path.string().c_str(), a_w, a_h, 4, data, a_w * 4);
            break;
        }
        case SF_TGA:
        {
            path.replace_extension(".tga");
            result = stbi_write_tga(path.string().c_str(), a_w, a_h, 4, data);
            break;
        }
        case SF_JPG:
        {
            path.replace_extension(".jpg");
            result = stbi_write_jpg(path.string().c_str(), a_w, a_h, 4, data, 100);
            break;
        }
        default:
        {
            path.replace_extension(".bmp");
            result = stbi_write_bmp(path.string().c_str(), a_w, a_h, 4, data);
            break;
        }
    }
    if (result == 0)
    {
        g_Orion.CreateTextMessageF(3, 0, "Failed to write screenshot");
        return;
    }

    if (g_GameState >= GS_GAME)
    {
        g_Orion.CreateTextMessageF(3, 0, "Screenshot saved to: %s", path.string().c_str());
    }
}
