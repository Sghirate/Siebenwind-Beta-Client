#pragma once

#include "Gump.h"
#include "../TextEngine/TextRenderer.h"

class CGumpContainer : public CGump
{
    bool IsGameBoard = false;

private:
    u32 m_CorpseEyesTicks{ 0 };
    u8 m_CorpseEyesOffset{ 0 };

    CTextRenderer m_TextRenderer{ CTextRenderer() };

    CGUIGumppic *m_CorpseEyes{ nullptr };
    CGUIDataBox *m_DataBox{ nullptr };

protected:
    virtual void CalculateGumpState();

public:
    CGumpContainer(u32 serial, u32 id, short x, short y);
    virtual ~CGumpContainer();

    static const u32 ID_GC_LOCK_MOVING;
    static const u32 ID_GC_MINIMIZE;

    CGUIGumppic *m_BodyGump{ nullptr };

    void UpdateItemCoordinates(class CGameObject *item);

    CTextRenderer *GetTextRenderer() { return &m_TextRenderer; }

    virtual void PrepareTextures();
    virtual void PrepareContent();
    virtual void UpdateContent();
    virtual void InitToolTip();
    virtual void Draw();
    virtual CRenderObject *Select();

    GUMP_BUTTON_EVENT_H;

    virtual void OnLeftMouseButtonUp();
    virtual bool OnLeftMouseButtonDoubleClick();
};
