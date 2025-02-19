#pragma once

#include "plugin/enumlist.h"
#include <GL/glew.h>

class CScreenEffectManager
{
public:
    SCREEN_EFFECT_MODE Mode = SEM_NONE;
    SCREEN_EFFECT_TYPE Type = SET_TO_BLACK;
    bool Enabled            = false;
    GLfloat ColorR          = 0.0f;
    GLfloat ColorG          = 0.0f;
    GLfloat ColorB          = 0.0f;
    GLfloat Alpha           = 1.0f;
    GLfloat Step            = 0.03f;

    CScreenEffectManager();
    virtual ~CScreenEffectManager();

    int Process();
    void Draw();
    bool
    Use(const SCREEN_EFFECT_MODE& mode,
        const SCREEN_EFFECT_TYPE& type = SET_TO_BLACK,
        bool ignoreEnabled             = false);
    bool UseSunrise();
    bool UseSunset();
};

extern CScreenEffectManager g_ScreenEffectManager;
