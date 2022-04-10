// MIT License
// Copyright (C) September 2016 Hotride

#pragma once

#include "Gump.h"
#include "../Platform.h"

class CGumpBook : public CGump
{
private:
    bool *m_ChangedPage{ nullptr };
    bool *m_PageDataReceived{ nullptr };
    bool WasAtEnd = false;

    void InsertInContent(const Keycode key, bool isCharPress = true);

    CGUIButton *m_PrevPage{ nullptr };
    CGUIButton *m_NextPage{ nullptr };

    void SetPagePos(int val, int page);

public:
    bool Writable = false;
    u16 PageCount = 0;
    bool Unicode = false;

    CGumpBook(
        u32 serial, i16 x, i16 y, u16 pageCount, bool writable, bool unicode);
    virtual ~CGumpBook();

    virtual void PasteClipboardData(wstring &data) override;

    CGUITextEntry *m_EntryAuthor{ nullptr };
    CGUITextEntry *m_EntryTitle{ nullptr };
    CGUITextEntry *GetEntry(int page);

    virtual void DelayedClick(CRenderObject *obj);
    virtual void PrepareContent();

    void SetPageData(int page, const std::wstring &data);
    void ChangePage(int newPage, bool playSound = true);

    GUMP_BUTTON_EVENT_H;

    bool OnLeftMouseButtonDoubleClick();

    virtual void OnTextInput(const TextEvent &ev) override;
    virtual void OnKeyDown(const KeyEvent &ev) override;
};
