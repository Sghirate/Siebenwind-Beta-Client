#pragma once

class CScreenshotBuilder
{
public:
    CScreenshotBuilder();
    virtual ~CScreenshotBuilder();

    void SaveScreen();
    void SaveScreen(int a_x, int a_y, int a_w, int a_h);
};

extern CScreenshotBuilder g_ScreenshotBuilder;
