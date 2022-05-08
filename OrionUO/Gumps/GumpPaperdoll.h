#pragma once

#include "Gump.h"
#include "TextEngine/TextRenderer.h"
#include "TextEngine/TextContainer.h"

class CGumpPaperdoll : public CGump
{
protected:
    static const int m_LayerCount = 23;
    static int UsedLayers[m_LayerCount];

    CTextRenderer m_TextRenderer{ CTextRenderer() };
    CGUIDataBox *m_DataBox{ nullptr };
    CGUIText *m_Description{ nullptr };
    bool m_WantTransparentContent = false;

     void CalculateGumpState()  override;

public:
    static const int ID_GP_ITEMS = 20;

    CGumpPaperdoll(u32 serial, short x, short y, bool minimized);
    virtual ~CGumpPaperdoll();

    bool CanLift = false;
    CGUIButton *m_ButtonWarmode{ nullptr };
    CTextContainer m_TextContainer{ CTextContainer(10) };

    CTextRenderer *GetTextRenderer() { return &m_TextRenderer; }
    void UpdateDescription(const std::string &text);
     void DelayedClick(CRenderObject *obj)  override;
     void PrepareContent()  override;
     void UpdateContent()  override;
     void InitToolTip()  override;
     void Draw()  override;
     CRenderObject *Select()  override;

    GUMP_BUTTON_EVENT_H;
     void OnLeftMouseButtonUp()  override;
     bool OnLeftMouseButtonDoubleClick()  override;
};
