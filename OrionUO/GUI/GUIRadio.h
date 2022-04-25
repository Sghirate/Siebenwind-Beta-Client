#pragma once

#include "GUICheckbox.h"

class CGUIRadio : public CGUICheckbox
{
public:
    CGUIRadio(
        int serial,
        u16 graphic,
        u16 graphicChecked,
        u16 graphicDisabled,
        int x,
        int y);
    virtual ~CGUIRadio();
};
