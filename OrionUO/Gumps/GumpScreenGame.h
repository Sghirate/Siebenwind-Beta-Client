#pragma once

#include "Gump.h"

class CGumpScreenGame : public CGump
{
private:
    static const int ID_GS_RESIZE = 1;

public:
    CGumpScreenGame();
    virtual ~CGumpScreenGame();

     void UpdateContent()  override;
     void InitToolTip()  override;
     void Draw()  override;
     CRenderObject *Select()  override;
     void OnLeftMouseButtonDown()  override;
     void OnLeftMouseButtonUp()  override;
};
