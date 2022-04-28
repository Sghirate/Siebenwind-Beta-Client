#pragma once

#include <filesystem>
#include <vector>
#include <unordered_map>

class CGameItem;
class CMultiObject;

class CBuildObject
{
public:
    u16 Graphic = 0;
    char X = 0;
    char Y = 0;
    char Z = 0;

    CBuildObject() {}
    CBuildObject(u16 graphic, char x, char y, char z)
        : Graphic(graphic)
        , X(x)
        , Y(y)
        , Z(z)
    {
    }
    ~CBuildObject() {}
};

class CCustomHouse
{
public:
    u32 Serial = 0;
    u32 Revision = 0;

    CCustomHouse() {}
    CCustomHouse(int serial, int revision)
        : Serial(serial)
        , Revision(revision)
    {
    }
    ~CCustomHouse() {}

    std::vector<CBuildObject> m_Items;

    void Paste(CGameItem *foundation);
};

class CustomHousesManager : public Core::StreamReader
{
    std::unordered_map<u32, CCustomHouse *> m_Items;

public:
    CustomHousesManager() {}
    ~CustomHousesManager();

    void Clear();

    CCustomHouse *Get(int serial);

    void Add(CCustomHouse *house);

    void Load(const std::filesystem::path& a_path);
    void Save(const std::filesystem::path& a_path);
};

extern CustomHousesManager g_CustomHousesManager;
