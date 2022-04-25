#pragma once

class CCity
{
public:
    string Name = {};
    std::wstring Description = {};

    CCity(const std::string &name, const std::wstring &description);
    CCity() = default;
    virtual ~CCity();
};

class CCityManager
{
public:
    std::deque<CCity> m_CityList;

    CCityManager();
    virtual ~CCityManager();

    void Init();
    void Clear();
    CCity GetCity(const std::string &name);
};

extern CCityManager g_CityManager;
