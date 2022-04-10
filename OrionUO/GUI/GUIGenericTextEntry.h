// MIT License
// Copyright (C) August 2016 Hotride

#pragma once

#include "GUITextEntry.h"

class CGUIGenericTextEntry : public CGUITextEntry
{
public:
    //!Индекс текста
    u32 TextID = 0;

    CGUIGenericTextEntry(
        int serial, int index, u16 color, int x, int y, int maxWidth = 0, int maxLength = 0);
    virtual ~CGUIGenericTextEntry();
};
