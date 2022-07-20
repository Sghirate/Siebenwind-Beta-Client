#pragma once

#include "Core/Minimal.h"

class CClickObject
{
public:
    u32 Timer                   = 0;
    u16 X                       = 0;
    u16 Y                       = 0;
    bool Enabled                = false;
    u32 Page                    = 0;
    class CRenderObject* Object = nullptr;
    class CGump* Gump           = nullptr;
    CClickObject() {}
    ~CClickObject() {}

    void Clear(class CRenderObject* obj);
    void Clear();

    void Init(class CRenderObject* obj, class CGump* gump = nullptr);
};
extern CClickObject g_ClickObject;
