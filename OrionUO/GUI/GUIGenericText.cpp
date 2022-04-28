#include "GUIGenericText.h"
#include "GameVars.h"
#include "../Config.h"
#include "../DefinitionMacro.h"

CGUIGenericText::CGUIGenericText(int index, u16 color, int x, int y, int maxWidth)
    : CGUIText(color, x, y)
    , TextID(index)
    , MaxWidth(maxWidth)
{
}

CGUIGenericText::~CGUIGenericText()
{
}

void CGUIGenericText::CreateTexture(const std::wstring& str)
{
    u16 flags = UOFONT_BLACK_BORDER;

    if (MaxWidth != 0)
    {
        flags |= UOFONT_CROPPED;
    }

    CreateTextureW(
        (u8)(GameVars::GetClientVersion() >= CV_305D), str, 30, MaxWidth, TS_LEFT, flags);
}
