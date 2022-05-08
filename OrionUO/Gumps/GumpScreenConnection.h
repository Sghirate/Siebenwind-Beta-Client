#pragma once

#include "Core/Minimal.h"
#include "Gump.h"
#include <string>

class CGumpScreenConnection : public CGump
{
private:
    static const int ID_CS_OK     = 1;
    static const int ID_CS_CANCEL = 2;

    void CreateText(int x, int y, std::string str, u8 font);

public:
    CGumpScreenConnection();
    virtual ~CGumpScreenConnection();
    virtual void UpdateContent();

    GUMP_BUTTON_EVENT_H;
};
