#include "CustomHousesManager.h"
#include "Core/DataStream.h"
#include "../Gumps/GumpCustomHouse.h"
#include "../GameObjects/GameItem.h"

CustomHousesManager g_CustomHousesManager;

void CCustomHouse::Paste(CGameItem* foundation)
{
    if (foundation == nullptr)
    {
        return;
    }

    foundation->ClearCustomHouseMultis(0);
    int z = foundation->GetZ();

    for (const CBuildObject& item : m_Items)
    {
        foundation->AddMulti(item.Graphic, 0, item.X, item.Y, item.Z + z, true);
    }

    if (g_CustomHouseGump != nullptr && g_CustomHouseGump->Serial == Serial)
    {
        g_CustomHouseGump->WantUpdateContent = true;
        g_CustomHouseGump->GenerateFloorPlace();
    }
}

CustomHousesManager::~CustomHousesManager()
{
    Clear();
}

void CustomHousesManager::Clear()
{
    for (unordered_map<u32, CCustomHouse*>::iterator i = m_Items.begin(); i != m_Items.end();
         ++i)
    {
        CCustomHouse* house = i->second;
        delete house;
        i->second = nullptr;
    }
}

CCustomHouse* CustomHousesManager::Get(int serial)
{
    for (unordered_map<u32, CCustomHouse*>::iterator i = m_Items.begin(); i != m_Items.end();
         ++i)
    {
        CCustomHouse* house = i->second;

        if (house != nullptr && house->Serial == serial)
        {
            return i->second;
        }
    }

    return nullptr;
}

void CustomHousesManager::Add(CCustomHouse* house)
{
    if (house != nullptr)
    {
        m_Items[house->Serial] = house;
    }
}

void CustomHousesManager::Load(const os_path& path)
{
    Clear();
    Core::MappedFile file;
    if (file.Load(a_path) && (file.GetSize() != 0u))
    {
        u8 version = file.ReadLE<u8>();
        int count  = file.ReadLE<i32>();
        for (int i = 0; i < count; i++)
        {
            u32 serial = file.ReadLE<u32>();
            if (serial == 0u)
                break;

            u32 revision   = file.ReadLE<u32>();
            int itemsCount = file.ReadLE<i32>();

            CCustomHouse* house = Get(serial);

            LOG("Load house from cache file: 0x%08X 0x%08X", serial, revision);

            if (house == nullptr)
            {
                house = new CCustomHouse(serial, revision);
                g_CustomHousesManager.Add(house);
            }
            else
            {
                house->Revision = revision;
            }

            for (int j = 0; j < itemsCount; j++)
            {
                u16 graphic = file.ReadLE<u16>();
                char x      = file.ReadLE<i8>();
                char y      = file.ReadLE<i8>();
                char z      = file.ReadLE<i8>();

                house->m_Items.push_back(CBuildObject(graphic, x, y, z));
            }
        }
        file.Unload();
    }
}

void CustomHousesManager::Save(const std::filesystem::path& a_path)
{
    Core::StreamWriter writer;
    writer.WriteLE((i8)1);
    int count = 0;
    for (std::unordered_map<u32, CCustomHouse*>::iterator i = m_Items.begin(); i != m_Items.end();
         ++i)
    {
        CCustomHouse* house = i->second;
        if (house != nullptr && (static_cast<unsigned int>(!house->m_Items.empty()) != 0u))
            count++;
    }
    writer.WriteLE((i32)count);
    for (std::unordered_map<u32, CCustomHouse*>::iterator i = m_Items.begin(); i != m_Items.end();
         ++i)
    {
        CCustomHouse* house = i->second;

        if (house == nullptr || house->m_Items.empty())
            continue;

        writer.WriteLE((u32)house->Serial);
        writer.WriteLE((u32)house->Revision);
        writer.WriteLE((i32)house->m_Items.size());
        for (const CBuildObject& item : house->m_Items)
        {
            writer.WriteLE((u16)item.Graphic);
            writer.WriteLE((i8)item.X);
            writer.WriteLE((i8)item.Y);
            writer.WriteLE((i8)item.Z);
        }
    }
    writer.WriteLE((u32)0);
    Core::File file(a_path, "wb");
    file.Write(writer.GetBuffer(), writer.GetSize(), 1);
}
