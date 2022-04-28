#include "GUIGenericTextEntry.h"
#include "GameVars.h"
#include "../Config.h"
#include "../DefinitionMacro.h"

CGUIGenericTextEntry::CGUIGenericTextEntry(
    int serial, int index, u16 color, int x, int y, int maxWidth, int maxLength)
    : CGUITextEntry(
          serial,
          color,
          color,
          color,
          x,
          y,
          maxWidth,
          true,
          (u8)(GameVars::GetClientVersion() >= CV_305D),
          TS_LEFT,
          UOFONT_BLACK_BORDER,
          maxLength)
    , TextID(index)
{
}

CGUIGenericTextEntry::~CGUIGenericTextEntry()
{
}
