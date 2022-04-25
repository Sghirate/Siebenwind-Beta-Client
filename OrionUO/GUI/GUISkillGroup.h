﻿#pragma once

#include "BaseGUI.h"
class CGUIButton;
class CGUITextEntry;

class CGUISkillGroup : public CBaseGUI
{
    //!Свернута или развернута группа
protected:
    bool m_Minimized = false;

public:
    bool GetMinimized() { return m_Minimized; };
    void SetMinimized(bool val);

public:
    CGUISkillGroup(int serial, int minimizeSerial, class CSkillGroupObject *group, int x, int y);
    virtual ~CGUISkillGroup();

    //!Компоненты управления
    CGUIButton *m_Minimizer{ nullptr };
    CGUITextEntry *m_Name{ nullptr };

    void UpdateDataPositions();

    virtual Core::Vec2<i32> GetSize();

    virtual void PrepareTextures();

    virtual bool EntryPointerHere();

    virtual void Draw(bool checktrans = false);
    virtual bool Select();

    //!Получить ссылку на выбранную компоненту или на себя
    CBaseGUI *SelectedItem();
};
