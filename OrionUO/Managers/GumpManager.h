#pragma once

#include "Core/Minimal.h"
#include "Core/DataStream.h"
#include "Platform.h"
#include "BaseQueue.h"
#include "plugin/enumlist.h"
#include <filesystem>

namespace Core { struct KeyEvent; struct TextEvent; }

class CGumpManager : public CBaseQueue
{
private:
    int GetNonpartyStatusbarsCount();
    void SaveDefaultGumpProperties(Core::StreamWriter& a_writer, class CGump* a_gump, int a_size);

public:
    CGumpManager()
        : CBaseQueue()
    {
    }
    virtual ~CGumpManager() {}

    void AddGump(CGump* obj);
    CGump* UpdateContent(int serial, int id, const GUMP_TYPE& type);
    CGump* UpdateGump(int serial, int id, const GUMP_TYPE& type);
    CGump* GetGump(int serial, int id, const GUMP_TYPE& type);
    CGump* GetTextEntryOwner();
    CGump* GumpExists(uintptr_t gumpID);
    void CloseGump(u32 serial, u32 ID, GUMP_TYPE Type);
    void RemoveGump(CGump* obj);
    void RedrawAll();
    void OnDelete();
    void RemoveRangedGumps();
    void PrepareContent();
    void RemoveMarked();
    void PrepareTextures();
    void Draw(bool blocked);
    void Select(bool blocked);
    void InitToolTip();
    void OnLeftMouseButtonDown(bool blocked);
    bool OnLeftMouseButtonUp(bool blocked);
    bool OnLeftMouseButtonDoubleClick(bool blocked);
    void OnRightMouseButtonDown(bool blocked);
    void OnRightMouseButtonUp(bool blocked);
    bool OnRightMouseButtonDoubleClick(bool blocked) { return false; }
    void OnMidMouseButtonScroll(bool up, bool blocked);
    void OnDragging(bool blocked);
    void Load(const std::filesystem::path& a_path);
    void Save(const std::filesystem::path& a_path);
    virtual bool OnTextInput(const Core::TextEvent& ev, bool blocked);
    virtual bool OnKeyDown(const Core::KeyEvent& ev, bool blocked);
};

extern CGumpManager g_GumpManager;
