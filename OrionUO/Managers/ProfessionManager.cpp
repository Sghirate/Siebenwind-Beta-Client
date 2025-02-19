#include "ProfessionManager.h"
#include "ClilocManager.h"
#include "Core/Log.h"
#include "Core/StringUtils.h"
#include "GameVars.h"
#include "Globals.h"
#include "SkillsManager.h"
#include "Config.h"
#include "OrionUO.h"
#include "OrionApplication.h"
#include "Profession.h"

CProfessionManager g_ProfessionManager;

const std::string CProfessionManager::m_Keys[m_KeyCount] = {
    "begin", "name", "truename", "desc", "toplevel", "gump", "type",     "children", "skill",
    "stat",  "str",  "int",      "dex",  "end",      "true", "category", "nameid",   "descid"
};

CProfessionManager::CProfessionManager()
    : Selected(nullptr)
{
}

CProfessionManager::~CProfessionManager()
{
}

int CProfessionManager::GetKeyCode(const std::string &key)
{
    std::string str = Core::ToLowerA(key);
    int result = 0;

    for (int i = 0; i < m_KeyCount && (result == 0); i++)
    {
        if (str == m_Keys[i])
        {
            result = (int)i + 1;
        }
    }

    return result;
}

bool CProfessionManager::ParseFilePart(Core::TextFileParser &file)
{
    PROFESSION_TYPE type = PT_NO_PROF;
    std::vector<std::string> childrens;
    std::string name{};
    std::string trueName{};
    u32 nameClilocID = 0;
    u32 descriptionClilocID = 0;
    int descriptionIndex = 0;
    u16 gump = 0;
    bool topLevel = false;
    int skillCount = 0;
    int skillIndex[4] = { 0xFF, 0xFF, 0xFF, 0xFF };
    int skillValue[4] = { 0, 0, 0, 0 };
    int stats[3] = { 0 };

    bool exit = false;
    while (!file.IsEOF() && !exit)
    {
        std::vector<std::string> strings = file.ReadTokens();

        if (strings.empty())
        {
            continue;
        }

        int code = GetKeyCode(strings[0]);
        switch (code)
        {
            case PM_CODE_BEGIN:
            case PM_CODE_END:
            {
                exit = true;
                break;
            }
            case PM_CODE_NAME:
            {
                name = strings[1];
                break;
            }
            case PM_CODE_TRUENAME:
            {
                trueName = strings[1];
                break;
            }
            case PM_CODE_DESC:
            {
                descriptionIndex = atoi(strings[1].c_str());
                break;
            }
            case PM_CODE_TOPLEVEL:
            {
                topLevel = (GetKeyCode(strings[1]) == PM_CODE_TRUE);
                break;
            }
            case PM_CODE_GUMP:
            {
                gump = atoi(strings[1].c_str());

                g_Orion.ExecuteGump(gump);
                g_Orion.ExecuteGump(gump + 1);
                break;
            }
            case PM_CODE_TYPE:
            {
                if (GetKeyCode(strings[1]) == PM_CODE_CATEGORY)
                {
                    type = PT_CATEGORY;
                }
                else
                {
                    type = PT_PROFESSION;
                }

                break;
            }
            case PM_CODE_CHILDREN:
            {
                for (int j = 1; j < (int)strings.size(); j++)
                {
                    childrens.push_back(strings[j]);
                }

                break;
            }
            case PM_CODE_SKILL:
            {
                if (strings.size() > 2 && skillCount < 4)
                {
                    for (int j = 0; j < 54; j++)
                    {
                        CSkill *skillPtr = g_SkillsManager.Get((u32)j);

                        if (skillPtr != nullptr && strings[1] == skillPtr->Name)
                        {
                            skillIndex[skillCount] = (int)j;
                            skillValue[skillCount] = atoi(strings[2].c_str());
                            skillCount++;

                            break;
                        }
                    }
                }

                break;
            }
            case PM_CODE_STAT:
            {
                if (strings.size() > 2)
                {
                    code = GetKeyCode(strings[1]);
                    int val = atoi(strings[2].c_str());

                    if (code == PM_CODE_STR)
                    {
                        stats[0] = val;
                    }
                    else if (code == PM_CODE_INT)
                    {
                        stats[1] = val;
                    }
                    else if (code == PM_CODE_DEX)
                    {
                        stats[2] = val;
                    }
                }

                break;
            }
            case PM_CODE_NAME_CLILOC_ID:
            {
                nameClilocID = atoi(strings[1].c_str());
                name = Core::ToUpperA(g_ClilocManager.GetCliloc(g_Language)->GetA(nameClilocID, true, name));
                break;
            }
            case PM_CODE_DESCRIPTION_CLILOC_ID:
            {
                descriptionClilocID = atoi(strings[1].c_str());
                break;
            }
            default:
                break;
        }
    }

    CBaseProfession *obj = nullptr;

    if (type == PT_CATEGORY)
    {
        CProfessionCategory *temp = new CProfessionCategory();

        for (int i = 0; i < (int)childrens.size(); i++)
        {
            temp->AddChildren(childrens[i]);
        }

        obj = temp;
    }
    else if (type == PT_PROFESSION)
    {
        CProfession *temp = new CProfession();

        temp->Str = stats[0];
        temp->Int = stats[1];
        temp->Dex = stats[2];

        for (int i = 0; i < 4; i++)
        {
            temp->SetSkillIndex((int)i, (u8)skillIndex[i]);
            temp->SetSkillValue((int)i, (u8)skillValue[i]);
        }

        obj = temp;
    }

    bool result = (type != PROFESSION_TYPE::PT_NO_PROF);

    if (obj != nullptr)
    {
        obj->NameClilocID = nameClilocID;
        obj->Name = name;
        obj->TrueName = trueName;
        obj->DescriptionClilocID = descriptionClilocID;
        obj->DescriptionIndex = descriptionIndex;
        obj->TopLevel = topLevel;
        obj->Gump = gump;
        obj->Type = type;

        if (topLevel)
        {
            m_Items->Add(obj);
        }
        else
        {
            CBaseProfession *parent = (CBaseProfession *)m_Items;

            while (parent != nullptr)
            {
                result = AddChild(parent, obj);

                //if (result)
                //	break;

                parent = (CBaseProfession *)parent->m_Next;
            }

            if (!result)
            {
                delete obj;
            }
        }
    }

    return result;
}

bool CProfessionManager::AddChild(CBaseProfession *parent, CBaseProfession *child)
{
    bool result = false;

    if (parent->Type == PT_CATEGORY)
    {
        CProfessionCategory *cat = (CProfessionCategory *)parent;

        std::string check = std::string("|") + child->Name + "|";

        if (cat->Childrens.find(check) != std::string::npos)
        {
            cat->Add(child);
            result = true;
        }
        else
        {
            CBaseProfession *item = (CBaseProfession *)cat->m_Items;

            while (item != nullptr)
            {
                result = AddChild(item, child);

                if (result)
                {
                    break;
                }

                item = (CBaseProfession *)item->m_Next;
            }
        }
    }

    return result;
}

bool CProfessionManager::Load()
{
    bool result = false;

    CProfessionCategory *head = new CProfessionCategory();
    head->TrueName = "parent";
    head->Name = "Parent";
    head->DescriptionIndex = -2;
    head->Type = PT_CATEGORY;
    head->Gump = 0x15A9;
    head->TopLevel = true;
    Add(head);

    Core::TextFileParser file(g_App.GetGameDir() / "Prof.txt", " \t,", "#;", "\"\"");
    if (!file.IsEOF())
    {
        while (!file.IsEOF())
        {
            auto strings = file.ReadTokens();
            if (!strings.empty())
            {
                if (Core::ToLowerA(strings[0]) == std::string("begin"))
                {
                    result = ParseFilePart(file);

                    if (!result)
                    {
                        break;
                    }
                }
            }
        }

        g_Orion.ExecuteGump(0x15A9);
        g_Orion.ExecuteGump(0x15AA);

        CProfession *apc = new CProfession();
        apc->TrueName = "advanced";
        apc->Name = "Advanced";
        apc->Type = PT_PROFESSION;
        apc->Gump = 0x15A9;
        apc->DescriptionIndex = -1;
        apc->SetSkillIndex(0, 0xFF);
        apc->SetSkillIndex(1, 0xFF);
        apc->SetSkillIndex(2, 0xFF);
        apc->SetSkillIndex(3, 0xFF);

        if (GameVars::GetClientVersion() >= CV_70160)
        {
            apc->Str = 45;
            apc->Int = 35;
            apc->Dex = 10;

            apc->SetSkillValue(0, 30);
            apc->SetSkillValue(1, 30);
            apc->SetSkillValue(2, 30);
            apc->SetSkillValue(3, 30);
        }
        else
        {
            apc->Str = 44;
            apc->Int = 10;
            apc->Dex = 11;

            apc->SetSkillValue(0, 50);
            apc->SetSkillValue(1, 50);
            apc->SetSkillValue(2, 0);
            apc->SetSkillValue(3, 0);
        }

        head->Add(apc);

        LoadProfessionDescription();
    }
    else
    {
        LOG_ERROR("ProfessionManager", "Could not find prof.txt in your UO directory. Character creation professions loading failed.");
    }

    return result;
}

void CProfessionManager::LoadProfessionDescription()
{
    Core::MappedFile file;
    if (file.Load(g_App.GetGameDir() / "Professn.enu"))
    {
        char *ptr = (char *)file.GetBuffer();
        char *end = (char *)((uintptr_t)file.GetBuffer() + file.GetSize());
        std::vector<std::string> list;
        while (ptr < end)
        {
            if (memcmp(ptr, "TEXT", 4) == 0)
            {
                ptr += 8;

                while (ptr < end)
                {
                    if (((*(ptr - 1)) == 0) && ((*ptr) == 0)) //end of names section
                    {
                        ptr++;

                        break;
                    }

                    list.push_back(ptr);
                    ptr += strlen(ptr) + 1;
                }

                ((CBaseProfession *)m_Items)->AddDescription(-2, "parent", ptr);
                ptr += strlen(ptr) + 1;

                for (int i = 0; i < (int)list.size(); i++)
                {
                    if (!((CBaseProfession *)m_Items)->AddDescription((int)i - 1, list[i], ptr))
                    {
                        //LOG("Failed to add description! (%s)\n", list[i].c_str());
                    }
                    ptr += strlen(ptr) + 1;
                }

                break;
            }

            ptr++;
        }

        list.clear();

        file.Unload();
    }
    else
    {
        LOG_ERROR("ProfessionManager", "Failed to load professn.enu");
    }
}

CBaseProfession *CProfessionManager::GetParent(CBaseProfession *obj, CBaseProfession *check)
{
    if (check == nullptr)
    {
        check = (CBaseProfession *)m_Items;
    }

    if (obj == m_Items)
    {
        return obj;
    }

    CBaseProfession *item = (CBaseProfession *)check->m_Items;
    CBaseProfession *result = nullptr;

    while (item != nullptr && result == nullptr)
    {
        if (obj == item)
        {
            result = check;
        }
        else
        {
            result = GetParent(obj, item);
        }

        item = (CBaseProfession *)item->m_Next;
    }

    return result;
}
