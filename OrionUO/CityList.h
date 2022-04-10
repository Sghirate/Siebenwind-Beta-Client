// MIT License
// Copyright (C) August 2016 Hotride

#pragma once

#include "Managers/CityManager.h"

class CCityItem
{
public:
    u8 LocationIndex = 0;
    std::string Name = "";
    std::string Area = "";
    bool Selected = false;

    CCityItem();
    virtual ~CCityItem();

    CCity m_City;

    virtual bool IsNewCity() { return false; }
    void InitCity();
};

class CCityItemNew : public CCityItem
{
public:
    u32 X = 0;
    u32 Y = 0;
    u32 Z = 0;
    u32 MapIndex = 0;
    u32 Cliloc = 0;

    CCityItemNew();
    virtual ~CCityItemNew();

    bool IsNewCity() { return true; }
};

class CCityList
{
private:
    std::vector<CCityItem *> m_CityList;

public:
    CCityList();
    virtual ~CCityList();

    size_t CityCount() { return m_CityList.size(); }
    void AddCity(CCityItem *city) { m_CityList.push_back(city); }
    CCityItem *GetCity(int index);
    void Clear();
};

extern CCityList g_CityList;
