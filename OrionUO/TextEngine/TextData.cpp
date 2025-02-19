#include "TextData.h"
#include "Managers/ConfigManager.h"
#include "Managers/FontsManager.h"

CTextData::CTextData()
{
}

CTextData::CTextData(CTextData *obj)
    : Unicode(obj->Unicode)
    , Type(obj->Type)
    , Font(obj->Font)
    , Timer(obj->Timer)
    , Alpha(obj->Alpha)
{
    RealDrawX = obj->RealDrawX;
    RealDrawY = obj->RealDrawY;
    Color = obj->Color;
    Text = obj->Text;
    UnicodeText = obj->UnicodeText;
}

CTextData::~CTextData()
{
    m_Texture.Clear();
    Owner = nullptr;
}

bool CTextData::CanBeDrawedInJournalGump()
{
    bool result = true;

    switch (Type)
    {
        case TT_SYSTEM:
            result = g_JournalShowSystem;
            break;
        case TT_OBJECT:
            result = g_JournalShowObjects;
            break;
        case TT_CLIENT:
            result = g_JournalShowClient;
            break;
        default:
            break;
    }

    return result;
}

void CTextData::GenerateTexture(
    int maxWidth, u16 flags, TEXT_ALIGN_TYPE align, u8 cell, int font)
{
    if (Unicode)
    {
        if (font == -1)
        {
            font = Font;
        }

        g_FontManager.GenerateW(
            (u8)font, m_Texture, UnicodeText, Color, cell, maxWidth, align, flags);
    }
    else
    {
        g_FontManager.GenerateA((u8)Font, m_Texture, Text, Color, maxWidth, align, flags);
    }

    if (!m_Texture.Empty())
    {
        if (g_ConfigManager.ScaleSpeechDelay)
        {
            Timer += (((4000 * m_Texture.LinesCount) * g_ConfigManager.SpeechDelay) / 100);
        }
        else
        {
            u32 delay =
                ((i64)((i64)5497558140000 * g_ConfigManager.SpeechDelay) >> 32) >> 5;
            Timer += (u32)((delay >> 31) + delay);
        }
    }
}
