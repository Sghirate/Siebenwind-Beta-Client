// MIT License
// Copyright (C) August 2016 Hotride

#pragma once

#include "Wisp/WispApplication.h"

class COrionApplication : public Wisp::CApplication
{
#if USE_TIMERTHREAD
public:
    u32 NextRenderTime = 0;
    u32 NextUpdateTime = 0;
#endif // USE_TIMERTHREAD
protected:
    virtual void OnMainLoop();

public:
    COrionApplication()
        : Wisp::CApplication()
    {
    }
    virtual ~COrionApplication() {}
};

extern COrionApplication g_App;
