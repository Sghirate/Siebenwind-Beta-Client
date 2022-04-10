// MIT License
// Copyright (C) August 2016 Hotride

#pragma once

class CClickObject
{
public:
    u32 Timer = 0;
    u16 X = 0;
    u16 Y = 0;
    bool Enabled = false;
    u32 Page = 0;
    CRenderObject *Object = nullptr;
    CGump *Gump = nullptr;
    CClickObject() {}
    ~CClickObject() {}

    void Clear(CRenderObject *obj);
    void Clear();

    void Init(CRenderObject *obj, CGump *gump = nullptr);
};

extern CClickObject g_ClickObject; //Ссылка на объект клика
