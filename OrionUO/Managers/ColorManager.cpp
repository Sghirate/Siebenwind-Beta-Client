#include "ColorManager.h"
#include "Constants.h"
#include "FileManager.h"
#include "Globals.h"

CColorManager g_ColorManager;

CColorManager::CColorManager()
    : m_HuesCount(0)
{
}

CColorManager::~CColorManager()
{
}

void CColorManager::Init()
{
    intptr_t addr = (intptr_t)g_FileManager.m_HuesMul.GetBuffer();
    size_t size = g_FileManager.m_HuesMul.GetSize();

    if (addr > 0 && size > 0 && addr != -1 && size != -1)
    {
        size_t entryCount = size / sizeof(HUES_GROUP);

        m_HuesCount = (int)entryCount * 8;
        m_HuesRange.resize(entryCount);

        memcpy(&m_HuesRange[0], (void *)addr, entryCount * sizeof(HUES_GROUP));
    }
    else
    {
        m_HuesCount = 0;
    }

    if (g_FileManager.m_RadarcolMul.GetSize() != 0u)
    {
        m_Radarcol.resize(g_FileManager.m_RadarcolMul.GetSize() / 2);
        memcpy(
            &m_Radarcol[0],
            (void *)g_FileManager.m_RadarcolMul.GetBuffer(),
            g_FileManager.m_RadarcolMul.GetSize());
    }
}

void CColorManager::SetHuesBlock(int index, VERDATA_HUES_GROUP *group)
{
    if (index < 0 || index >= m_HuesCount)
    {
        return;
    }

    m_HuesRange[index].Header = group->Header;
    for (int i = 0; i < 8; i++)
    {
        memcpy(
            &m_HuesRange[index].Entries[i].ColorTable[0],
            &group->Entries[i].ColorTable[0],
            sizeof(u16[32]));
    }
}

void CColorManager::CreateHuesPalette()
{
    m_HuesFloat.resize(m_HuesCount);
    int entryCount = m_HuesCount / 8;

    for (int i = 0; i < entryCount; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            FLOAT_HUES &fh = m_HuesFloat[(i * 8) + j];

            for (int h = 0; h < 32; h++)
            {
                int idx = (int)h * 3;

                u16 c = m_HuesRange[i].Entries[j].ColorTable[h];

                fh.Palette[idx] = (((c >> 10) & 0x1F) / 31.0f);
                fh.Palette[idx + 1] = (((c >> 5) & 0x1F) / 31.0f);
                fh.Palette[idx + 2] = ((c & 0x1F) / 31.0f);
            }
        }
    }
}

void CColorManager::SendColorsToShader(u16 color)
{
    if (color != 0)
    {
        if ((color & SPECTRAL_COLOR_FLAG) != 0)
        {
            glUniform1fv(ShaderColorTable, 32 * 3, &m_HuesFloat[0].Palette[0]);
        }
        else
        {
            if (color >= m_HuesCount)
            {
                color %= m_HuesCount;

                if (color == 0u)
                {
                    color = 1;
                }
            }

            glUniform1fv(ShaderColorTable, 32 * 3, &m_HuesFloat[color - 1].Palette[0]);
        }
    }
}

u32 CColorManager::Color16To32(u16 c)
{
    const u8 table[32] = { 0x00, 0x08, 0x10, 0x18, 0x20, 0x29, 0x31, 0x39, 0x41, 0x4A, 0x52,
                                0x5A, 0x62, 0x6A, 0x73, 0x7B, 0x83, 0x8B, 0x94, 0x9C, 0xA4, 0xAC,
                                0xB4, 0xBD, 0xC5, 0xCD, 0xD5, 0xDE, 0xE6, 0xEE, 0xF6, 0xFF };

    return (table[(c >> 10) & 0x1F] | (table[(c >> 5) & 0x1F] << 8) | (table[c & 0x1F] << 16));

    /*return
	(
		(((c >> 10) & 0x1F) * 0xFF / 0x1F) |
		((((c >> 5) & 0x1F) * 0xFF / 0x1F) << 8) |
		(((c & 0x1F) * 0xFF / 0x1F) << 16)
	);*/
}

u16 CColorManager::Color32To16(int c)
{
    return (((c & 0xFF) * 32) / 256) | (((((c >> 16) & 0xff) * 32) / 256) << 10) |
           (((((c >> 8) & 0xff) * 32) / 256) << 5);
}

u16 CColorManager::ConvertToGray(u16 c)
{
    return ((c & 0x1F) * 299 + ((c >> 5) & 0x1F) * 587 + ((c >> 10) & 0x1F) * 114) / 1000;
}

u16 CColorManager::GetColor16(u16 c, u16 color)
{
    if (color != 0 && color < m_HuesCount)
    {
        color -= 1;
        int g = color / 8;
        int e = color % 8;

        return m_HuesRange[g].Entries[e].ColorTable[(c >> 10) & 0x1F];
    }

    return c;
}

u16 CColorManager::GetRadarColorData(int c)
{
    if (c < (int)m_Radarcol.size())
    {
        return m_Radarcol[c];
    }

    return 0;
}

u32 CColorManager::GetPolygoneColor(u16 c, u16 color)
{
    if (color != 0 && color < m_HuesCount)
    {
        color -= 1;
        int g = color / 8;
        int e = color % 8;

        return Color16To32(m_HuesRange[g].Entries[e].ColorTable[c]);
    }

    return 0xFF010101; //Black
}

u32 CColorManager::GetUnicodeFontColor(u16 &c, u16 color)
{
    if (color != 0 && color < m_HuesCount)
    {
        color -= 1;
        int g = color / 8;
        int e = color % 8;

        return Color16To32(m_HuesRange[g].Entries[e].ColorTable[8]);
    }

    return Color16To32(c);
}

u32 CColorManager::GetColor(u16 &c, u16 color)
{
    if (color != 0 && color < m_HuesCount)
    {
        color -= 1;
        int g = color / 8;
        int e = color % 8;

        return Color16To32(m_HuesRange[g].Entries[e].ColorTable[(c >> 10) & 0x1F]);
    }

    return Color16To32(c);
}

u32 CColorManager::GetPartialHueColor(u16 &c, u16 color)
{
    if (color != 0 && color < m_HuesCount)
    {
        color -= 1;
        int g = color / 8;
        int e = color % 8;

        u32 cl = Color16To32(c);

        if (ToColorR(cl) == ToColorG(cl) && ToColorB(cl) == ToColorG(cl))
        {
            return Color16To32(m_HuesRange[g].Entries[e].ColorTable[(c >> 10) & 0x1F]);
        }

        return cl;
    }

    return Color16To32(c);
}

u16 CColorManager::FixColor(u16 color, u16 defaultColor)
{
    u16 fixedColor = color & 0x3FFF;

    if (fixedColor != 0u)
    {
        if (fixedColor >= 0x0BB8)
        {
            fixedColor = 1;
        }

        fixedColor |= (color & 0xC000);
    }
    else
    {
        fixedColor = defaultColor;
    }

    return fixedColor;
}
