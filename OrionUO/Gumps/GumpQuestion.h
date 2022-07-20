#ifndef GumpQuestionH
#define GumpQuestionH

#include "Core/Minimal.h"
#include "Gump.h"

class CGumpQuestion : public CGump
{
    u8 Variant = 0;

private:
    static const int ID_GQ_BUTTON_CANCEL = 1;
    static const int ID_GQ_BUTTON_OKAY   = 2;

public:
    CGumpQuestion(u32 serial, short x, short y, u8 variant);
    virtual ~CGumpQuestion();

    static const int ID_GQ_STATE_QUIT           = 1;
    static const int ID_GQ_STATE_ATTACK_REQUEST = 2;

    GUMP_BUTTON_EVENT_H;
};

#endif
