#pragma once

#include "Core/Time.h"
#include <map>

class CObjectProperty
{
public:
    u32 Serial = 0;
    u32 Revision = 0;
    std::wstring Name = {};
    std::wstring Data = {};

    CObjectProperty() {}
    CObjectProperty(int serial, int revision, const std::wstring &name, const std::wstring &data);
    bool Empty();
    std::wstring CreateTextData(bool extended);
};

typedef std::map<u32, CObjectProperty> OBJECT_PROPERTIES_MAP;

class CObjectPropertiesManager
{
public:
    CObjectPropertiesManager() {}
    virtual ~CObjectPropertiesManager();

    void Reset();
    bool RevisionCheck(int serial, int revision);
    void OnItemClicked(int serial);
    void Display(int serial);
    void Add(int serial, const CObjectProperty &objectProperty);
    void SetDuration(const Core::TimeDiff& a_duration) { m_showUntil = Core::FrameTimer::Now() + a_duration; }
    bool IsElapsed() const { return m_showUntil < Core::FrameTimer::Now(); }

private:
    OBJECT_PROPERTIES_MAP m_Map;
    class CRenderObject *m_Object{ nullptr };
    Core::TimeStamp m_showUntil;
};

extern CObjectPropertiesManager g_ObjectPropertiesManager;
