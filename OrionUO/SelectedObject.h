#pragma once

#include "Core/Minimal.h"

class CSelectedObject
{
public:
    u32 Serial                  = 0;
    class CRenderObject* Object = nullptr;
    class CGump* Gump           = nullptr;
    CSelectedObject();
    virtual ~CSelectedObject();

    void Clear();
    void Clear(class CRenderObject* obj);

    void Init(class CRenderObject* obj, class CGump* gump = nullptr);
    void Init(const class CSelectedObject& obj);
};

extern CSelectedObject g_SelectedObject;
extern CSelectedObject g_LastSelectedObject;
