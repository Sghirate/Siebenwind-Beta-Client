#pragma once

#include "Core/Minimal.h"
#include "Gump.h"

class CGumpMap : public CGump
{
    int m_PlotState = 0;
    u32 m_PinTimer = 0;
    CBaseGUI *m_PinOnCursor{ nullptr };
    CBaseGUI *m_FakeInsertionPin{ nullptr };
    CGUIButton *m_PlotCourse{ nullptr };
    CGUIButton *m_StopPlotting{ nullptr };
    CGUIButton *m_ClearCourse{ nullptr };
    std::vector<CGUIText *> m_Labels;

    int LineUnderMouse(int &x1, int &y1, int x2, int y2);

public:
    int StartX = 0;
    int StartY = 0;
    int EndX = 0;
    int EndY = 0;
    int Width = 0;
    int Height = 0;
    CGUIExternalTexture *m_Texture{ nullptr };
    CGUIDataBox *m_DataBox{ nullptr };

    CGumpMap(
        u32 serial,
        u16 graphic,
        int startX,
        int startY,
        int endX,
        int endY,
        int width,
        int height);
    virtual ~CGumpMap();

    int GetPlotState() { return m_PlotState; };
    void SetPlotState(int val);

     void PrepareContent()  override;
     void GenerateFrame(bool stop)  override;
     CRenderObject *Select()  override;

    GUMP_BUTTON_EVENT_H;

     void OnLeftMouseButtonDown()  override;
     void OnLeftMouseButtonUp()  override;
};
