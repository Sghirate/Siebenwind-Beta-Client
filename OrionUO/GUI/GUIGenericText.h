#pragma once

#include "Core/Minimal.h"
#include "GUIText.h"
#include <string>

class CGUIGenericText : public CGUIText
{
public:
    //!Индекс текста
    u32 TextID = 0;

    //!Максимальная ширина (для CroppedText)
    int MaxWidth = 0;

    CGUIGenericText(int index, u16 color, int x, int y, int maxWidth = 0);
    virtual ~CGUIGenericText();

    void CreateTexture(const std::wstring& str);
};
