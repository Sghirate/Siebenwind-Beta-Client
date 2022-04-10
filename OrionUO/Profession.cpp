#include "Profession.h"
#include "Core/StringUtils.h"
#include "Managers/FontsManager.h"

CBaseProfession::CBaseProfession()
{
}

CBaseProfession::~CBaseProfession()
{
}

bool CBaseProfession::AddDescription(int a_desc, const std::string& a_name, const char* a_val)
{
    bool result = (DescriptionIndex == a_desc);
    if (result)
    {
        if (NameClilocID == 0u)
            Name = a_name;

        g_FontManager.SetUseHTML(true);
        if (a_desc == -2)
            Description = Core::ToWString(a_val);
        else
            Description = Core::ToWString(a_name + "\n" + a_val);

        g_FontManager.SetUseHTML(false);
    }
    else
    {
        for (CBaseProfession* obj = (CBaseProfession*)m_Items; obj != nullptr && !result;
             obj                  = (CBaseProfession*)obj->m_Next)
            result = obj->AddDescription(a_desc, a_name, a_val);
    }
    return result;
}

CProfessionCategory::CProfessionCategory()
    : CBaseProfession()
{
}

CProfessionCategory::~CProfessionCategory()
{
}

void CProfessionCategory::AddChildren(const std::string& a_child)
{
    Childrens += a_child + "|";
}

CProfession::CProfession()
    : CBaseProfession()
{
    memset(&m_SkillIndex[0], 0, sizeof(m_SkillIndex));
    memset(&m_SkillValue[0], 0, sizeof(m_SkillValue));
}

CProfession::~CProfession()
{
}
