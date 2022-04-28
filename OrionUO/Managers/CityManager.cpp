#include "CityManager.h"
#include "Core/MappedFile.h"
#include "Core/StringUtils.h"
#include "ClilocManager.h"
#include "Globals.h"
#include "../OrionApplication.h"

CCityManager g_CityManager;

CCity::CCity(const std::string &name, const std::wstring &description)
    : Name(name)
    , Description(description)
{
}

CCity::~CCity()
{
}

CCityManager::CCityManager()
{
}

void CCityManager::Init()
{
    Core::MappedFile file;
    if (file.Load(g_App.GetGameDir() / "citytext.enu"))
    {
        u8* end = file.GetPtr() + file.GetSize();
        while (file.GetPtr() < end)
        {
            if (memcmp(&file.GetPtr()[0], "END\0", 4) == 0)
            {
                file.Move(4);
                u8* startBlock = file.GetPtr() + 4;
                u8* ptrBlock = startBlock;
                std::string name{};
                while (ptrBlock < end)
                {
                    if (*ptrBlock == '<')
                    {
                        size_t len = ptrBlock - startBlock;
                        name.resize(len);
                        memcpy(&name[0], &startBlock[0], len);
                        break;
                    }
                    ptrBlock++;
                }

                std::string text{};
                while (file.GetPtr() < end)
                {
                    std::string str = file.ReadString();
                    if (text.length() != 0u)
                        text += "\n\n";

                    text += str;
                    if (*file.GetPtr() == 0x2E || (memcmp(&file.GetPtr()[0], "END\0", 4) == 0))
                    {
                        break;
                    }
                }
                m_CityList.push_back(CCity(name, Core::ToWString(text)));
            }
            else
            {
                file.Move(1);
            }
        }
        file.Unload();
    }
    else
    {
        static const std::string cityNames[9] = { "Yew",      "Minoc",      "Britain",
                                                  "Moonglow", "Trinsic",    "Magincia",
                                                  "Jhelom",   "Skara Brae", "Vesper" };

        Cliloc* cliloc = g_ClilocManager.GetCliloc(g_Language);
        if (cliloc != nullptr)
        {
            for (int i = 0; i < (int)countof(cityNames); i++)
                m_CityList.push_back(CCity(cityNames[i], cliloc->GetW(1075072 + i)));
        }
    }
}

CCityManager::~CCityManager()
{
    Clear();
}

CCity CCityManager::GetCity(const std::string &name)
{

    for (auto &city : m_CityList)
    {
        if (city.Name == name)
        {
            return city;
        }
    }
    return CCity();
}

void CCityManager::Clear()
{
    m_CityList.clear();
}
