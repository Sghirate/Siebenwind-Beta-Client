#pragma once

#include "Core/App.h"

class COrionApplication : public Core::App
{
public:
    COrionApplication()
        : Core::App()
    {
    }
    virtual ~COrionApplication()
    {
    }

protected:
    void TickFrame() override;
    bool IsTerminating() const override;
};

extern COrionApplication g_App;
