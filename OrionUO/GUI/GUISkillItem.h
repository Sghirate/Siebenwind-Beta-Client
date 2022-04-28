#pragma once

#include "BaseGUI.h"
#include "../GLEngine/GLTextTexture.h"
class CGUIButton;

class CGUISkillItem : public CBaseGUI
{
public:
    //!Индекс навыка
    int Index = 0;

    //!Статус навыка
protected:
    u8 m_Status = 0;

public:
    u8 GetStatus() { return m_Status; };
    void SetStatus(u8 val);

private:
    //!Текустуры текста
    CGLTextTexture m_NameText{ CGLTextTexture() };
    CGLTextTexture m_ValueText{ CGLTextTexture() };

    //!Получить ИД картинки кнопки статуса
    u16 GetStatusButtonGraphic();

public:
    CGUISkillItem(int serial, int useSerial, int statusSerial, int index, int x, int y);
    virtual ~CGUISkillItem();

    //!Компоненты управления
    CGUIButton *m_ButtonUse{ nullptr };
    CGUIButton *m_ButtonStatus{ nullptr };

    //!Создать текстуру значения навыка
    void CreateValueText(bool showReal = false, bool showCap = false);

    virtual Core::Vec2<i32> GetSize() { return Core::Vec2<i32>(255, 17); }

    virtual void PrepareTextures();

    virtual void Draw(bool checktrans = false);
    virtual bool Select();

    //!Получить ссылку на выбранную компоненту или на себя
    CBaseGUI *SelectedItem();
};
