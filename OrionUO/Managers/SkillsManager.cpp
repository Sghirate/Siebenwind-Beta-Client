#include "SkillsManager.h"
#include "Core/Log.h"
#include "FileManager.h"
#include "plugin/mulstruct.h"

CSkillsManager g_SkillsManager;

CSkill::CSkill(bool haveButton, const std::string& name)
    : Button(haveButton)
{
    if (name.length() != 0u)
    {
        Name = name;
    }
    else
    {
        Name = "NoNameSkill";
    }

    //LOG("Skill loaded (button:%i): %s\n", m_Button, m_Name.c_str());
}

bool CSkillsManager::Load()
{
    if ((g_FileManager.m_SkillsIdx.GetSize() == 0u) || (g_FileManager.m_SkillsMul.GetSize() == 0u) ||
        (Count != 0u))
    {
        return false;
    }

    Core::MappedFile& idx = g_FileManager.m_SkillsIdx;
    Core::MappedFile& mul = g_FileManager.m_SkillsMul;
    while (!idx.IsEOF())
    {
        SKILLS_IDX_BLOCK* idxBlock = (SKILLS_IDX_BLOCK*)idx.GetPtr();
        idx.Move(sizeof(SKILLS_IDX_BLOCK));

        if ((idxBlock->Size != 0u) && idxBlock->Position != 0xFFFFFFFF &&
            idxBlock->Size != 0xFFFFFFFF)
        {
            mul.SetPtr(mul.GetBuffer() + idxBlock->Position);
            bool haveButton = (mul.ReadBE<u8>() != 0);
            Add(CSkill(haveButton, mul.ReadString(idxBlock->Size - 1)));
        }
    }

    LOG_INFO("SkillsManager", "Skills count: %i", Count);

    if (Count < 2 || Count > 100)
    {
        Count = 0;

        return false;
    }

    return true;
}

void CSkillsManager::Add(const CSkill& skill)
{
    m_Skills.push_back(skill);
    Count++;
}

void CSkillsManager::Clear()
{
    Count       = 0;
    SkillsTotal = 0.0f;
    m_Skills.clear();
    m_SortedTable.clear();
}

CSkill* CSkillsManager::Get(u32 index)
{
    if (index < Count)
    {
        return &m_Skills[index];
    }

    return nullptr;
}

bool CSkillsManager::CompareName(const std::string& str1, const std::string& str2)
{
    //Вычисляем минимальную длину строки для сравнения
    const auto len = (int)Core::Min(str1.length(), str2.length());

    bool result = false;

    //Пройдемся по всем символам этой строки, сравнивая их друг с другом
    for (int i = 0; i < len; i++)
    {
        char c1 = str1.at(i);
        char c2 = str2.at(i);

        if (c1 < c2)
        {
            return true;
        }
        if (c1 > c2)
        {
            return false;
        }
    }

    //Вернем что получилось
    return result;
}

void CSkillsManager::Sort()
{
    m_SortedTable.resize(Count, 0xFF);
    std::vector<u8> bufTable(Count, 0xFF);

    //Установим первый элемент нулем и количество обработанных навыков - 1
    int parsed  = 1;
    bufTable[0] = 0;

    //Пройдемся по всем нвыкам (кроме первого)
    for (u32 i = 1; i < Count; i++)
    {
        //Пройдемся по обработанным
        for (int j = 0; j < parsed; j++)
        {
            //Если можно вставить в текущую позицию -
            if (CompareName(m_Skills[bufTable[j]].Name, m_Skills[i].Name))
            {
                //Запомним индекс навыка
                u8 buf = bufTable[j];
                //Перезапишем
                bufTable[j] = (u8)i;

                //К следующему навыку
                j++;

                //Посмотрим остальные обработанные и перезапишем индекс при необходимости
                for (; j < parsed; j++)
                {
                    u8 ptr      = bufTable[j];
                    bufTable[j] = buf;
                    buf         = ptr;
                }

                //Запишем индекс в текущий обработанный
                bufTable[parsed] = buf;

                //Увеличиваем счетчик
                parsed++;

                break;
            }
        }
    }

    for (int i = 0, j = parsed - 1; i < parsed; i++, j--)
    {
        m_SortedTable[i] = bufTable[j];
    }
}

int CSkillsManager::GetSortedIndex(u32 index)
{
    if (index < Count)
    {
        return m_SortedTable[index];
    }

    return -1;
}

void CSkillsManager::UpdateSkillsSum()
{
    SkillsTotal = 0.0f;

    for (const CSkill& skill : m_Skills)
    {
        SkillsTotal += skill.Value;
    }
}
