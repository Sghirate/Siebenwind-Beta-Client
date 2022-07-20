#pragma once

#include "Core/Minimal.h"
#include "BaseGUI.h"
#include "GLEngine/GLTextTexture.h"

class CGUIButton;

class CGUISkillItem : public CBaseGUI
{
public:
    int Index = 0;

protected:
    u8 m_Status = 0;

public:
    u8 GetStatus() { return m_Status; };
    void SetStatus(u8 val);

private:
    CGLTextTexture m_NameText{ CGLTextTexture() };
    CGLTextTexture m_ValueText{ CGLTextTexture() };

    u16 GetStatusButtonGraphic();

public:
    CGUISkillItem(int serial, int useSerial, int statusSerial, int index, int x, int y);
    virtual ~CGUISkillItem();

    CGUIButton* m_ButtonUse{ nullptr };
    CGUIButton* m_ButtonStatus{ nullptr };

    void CreateValueText(bool showReal = false, bool showCap = false);
    virtual Core::Vec2<i32> GetSize() { return Core::Vec2<i32>(255, 17); }
    virtual void PrepareTextures();
    virtual void Draw(bool checktrans = false);
    virtual bool Select();
    CBaseGUI* SelectedItem();
};
