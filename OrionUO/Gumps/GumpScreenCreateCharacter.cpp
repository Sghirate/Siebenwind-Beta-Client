#include "GumpScreenCreateCharacter.h"
#include "GameVars.h"
#include "Config.h"
#include "ToolTip.h"
#include "SelectedObject.h"
#include "Managers/CreateCharacterManager.h"
#include "Managers/ConfigManager.h"
#include "Managers/ColorManager.h"
#include "ScreenStages/CreateCharacterScreen.h"

enum
{
    ID_CCS_NONE,

    ID_CCS_QUIT,
    ID_CCS_ARROW_PREV,
    ID_CCS_ARROW_NEXT,
    ID_CCS_NAME_FIELD,
    ID_CCS_MALE_BUTTON,
    ID_CCS_FEMALE_BUTTON,
    ID_CCS_HUMAN_RACE_BUTTON,
    ID_CCS_ELF_RACE_BUTTON,
    ID_CCS_GARGOYLE_RACE_BUTTON,

    ID_CCS_SKIN_TONE,
    ID_CCS_SHIRT_COLOR,
    ID_CCS_SKIRT_OR_PANTS_COLOR,
    ID_CCS_HAIR_COLOR,
    ID_CCS_FACIAL_HAIR_COLOR,

    ID_CCS_COUNT,

    ID_CCS_HAIR_STYLE = 20,
    ID_CCS_FACIAL_HAIR_STYLE = 40,
    ID_CCS_COLOR_RANGE = 100,
};

CGumpScreenCreateCharacter::CGumpScreenCreateCharacter()
    : CGump(GT_NONE, 0, 0, 0)
{
    NoMove = true;
    NoClose = true;
}

CGumpScreenCreateCharacter::~CGumpScreenCreateCharacter()
{
}

void CGumpScreenCreateCharacter::UpdateContent()
{
    Clear();

    Add(new CGUIGumppicTiled(0x0E14, 0, 0, 640, 480));
    Add(new CGUIGumppic(0x157C, 0, 0));
    Add(new CGUIGumppic(0x15A0, 0, 4));
    Add(new CGUIButton(ID_CCS_QUIT, 0x1589, 0x158A, 0x158B, 555, 4));
    Add(new CGUIButton(ID_CCS_ARROW_PREV, 0x15A1, 0x15A2, 0x15A3, 586, 445));
    Add(new CGUIButton(ID_CCS_ARROW_NEXT, 0x15A4, 0x15A5, 0x15A6, 610, 445));
    Add(new CGUIResizepic(0, 0x0E10, 82, 125, 151, 310));
    Add(new CGUIGumppic(0x0709, 280, 53));
    Add(new CGUIGumppic(0x070A, 240, 73));
    Add(new CGUIGumppicTiled(0x070B, 248, 73, 215, 0));
    Add(new CGUIGumppic(0x070C, 463, 73));

    CGUITextEntry *entry = (CGUITextEntry *)Add(
        new CGUITextEntry(ID_CCS_NAME_FIELD, 1, 1, 1, 257, 65, 300, false, 5, TS_LEFT, 0, 32));
    entry->CheckOnSerial = true;
    g_EntryPointer = &entry->m_Entry;
    g_EntryPointer->SetTextA(g_CreateCharacterScreen.Name);

    Add(new CGUIGumppic(0x0708, 238, 98));
    Add(new CGUIResizepic(0, 0x0E10, 475, 125, 151, 310));

    const u16 toneTextColorRange[2] = { 0x0481, 0x0021 };

    int facialHairCount = g_CreateCharacterManager.GetCurrentFacialHairCount();

    CGUIComboBox *combo = (CGUIComboBox *)Add(new CGUIComboBox(
        ID_CCS_FACIAL_HAIR_STYLE, 0x0BB8, false, 0x0BB8, 97, 199, 120, facialHairCount, false));
    combo->SelectedIndex = g_CreateCharacterManager.BeardStyle;
    combo->Visible =
        (!g_CreateCharacterManager.GetFemale() && g_CreateCharacterManager.GetRace() != RT_ELF);

    CGUIText *text = new CGUIText(0x0481, 3, -12);
    text->CreateTextureA(9, "Facial Hair Style");
    combo->SetText(text);

    for (int i = 0; i < facialHairCount; i++)
    {
        combo->Add(
            new CGUIComboboxText(0x0381, 9, g_CreateCharacterManager.GetBeard((u8)i).Name));
    }

    combo->RecalculateWidth();

    int hairCount = g_CreateCharacterManager.GetCurrentHairCount();

    combo = (CGUIComboBox *)Add(
        new CGUIComboBox(ID_CCS_HAIR_STYLE, 0x0BB8, false, 0x0BB8, 97, 154, 120, hairCount, false));
    combo->SelectedIndex = g_CreateCharacterManager.HairStyle;

    text = new CGUIText(0x0481, 3, -12);
    text->CreateTextureA(9, "Hair Style");
    combo->SetText(text);

    for (int i = 0; i < hairCount; i++)
    {
        combo->Add(
            new CGUIComboboxText(0x0381, 9, g_CreateCharacterManager.GetHair((u8)i).Name));
    }

    combo->RecalculateWidth();

    u8 *huesData = (u8 *)g_ColorManager.GetHuesRangePointer() + 32 + 4;
    int colorOffsetDivider = sizeof(HUES_GROUP) - 4;

    if (g_CreateCharacterScreen.GetColorSelection() == 0)
    {
        entry = (CGUITextEntry *)Add(new CGUITextEntry(
            ID_CCS_SKIN_TONE,
            toneTextColorRange[0],
            toneTextColorRange[1],
            toneTextColorRange[1],
            490,
            140,
            0,
            false,
            9));
        entry->m_Entry.SetTextA("Skin Tone");
        entry->CheckOnSerial = true;
        entry->ReadOnly = true;

        u16 color = g_CreateCharacterManager.SkinTone - 1;

        int colorIndex = (color + ((color + (color << 2)) << 1)) << 3;
        colorIndex += (colorIndex / colorOffsetDivider) << 2;
        color = *(u16 *)(huesData + colorIndex);

        u32 clr = g_ColorManager.Color16To32(color);

        Add(new CGUIColoredPolygone(ID_CCS_SKIN_TONE, color, 490, 154, 120, 25, clr));

        entry = (CGUITextEntry *)Add(new CGUITextEntry(
            ID_CCS_SHIRT_COLOR,
            toneTextColorRange[0],
            toneTextColorRange[1],
            toneTextColorRange[1],
            490,
            185,
            0,
            false,
            9));

        if (g_CreateCharacterManager.GetRace() == RT_GARGOYLE)
        {
            entry->m_Entry.SetTextA("Robe Color");
        }
        else
        {
            entry->m_Entry.SetTextA("Shirt Color");
        }

        entry->CheckOnSerial = true;
        entry->ReadOnly = true;

        color = g_CreateCharacterManager.ShirtColor - 1;

        colorIndex = (color + ((color + (color << 2)) << 1)) << 3;
        colorIndex += (colorIndex / colorOffsetDivider) << 2;
        color = *(u16 *)(huesData + colorIndex);

        clr = g_ColorManager.Color16To32(color);

        Add(new CGUIColoredPolygone(ID_CCS_SHIRT_COLOR, color, 490, 199, 120, 25, clr));

        if (g_CreateCharacterManager.GetRace() != RT_GARGOYLE)
        {
            entry = (CGUITextEntry *)Add(new CGUITextEntry(
                ID_CCS_SKIRT_OR_PANTS_COLOR,
                toneTextColorRange[0],
                toneTextColorRange[1],
                toneTextColorRange[1],
                490,
                230,
                0,
                false,
                9));
            entry->CheckOnSerial = true;
            entry->ReadOnly = true;

            if (g_CreateCharacterManager.GetFemale())
            {
                entry->m_Entry.SetTextA("Skirt Color");
            }
            else
            {
                entry->m_Entry.SetTextA("Pants Color");
            }

            color = g_CreateCharacterManager.PantsColor - 1;

            colorIndex = (color + ((color + (color << 2)) << 1)) << 3;
            colorIndex += (colorIndex / colorOffsetDivider) << 2;
            color = *(u16 *)(huesData + colorIndex);

            clr = g_ColorManager.Color16To32(color);

            Add(new CGUIColoredPolygone(
                ID_CCS_SKIRT_OR_PANTS_COLOR, color, 490, 244, 120, 25, clr));
        }

        entry = (CGUITextEntry *)Add(new CGUITextEntry(
            ID_CCS_HAIR_COLOR,
            toneTextColorRange[0],
            toneTextColorRange[1],
            toneTextColorRange[1],
            490,
            275,
            0,
            false,
            9));
        entry->m_Entry.SetTextA("Hair Color");
        entry->CheckOnSerial = true;
        entry->ReadOnly = true;

        color = g_CreateCharacterManager.HairColor - 1;

        colorIndex = (color + ((color + (color << 2)) << 1)) << 3;
        colorIndex += (colorIndex / colorOffsetDivider) << 2;
        color = *(u16 *)(huesData + colorIndex);

        clr = g_ColorManager.Color16To32(color);

        Add(new CGUIColoredPolygone(ID_CCS_HAIR_COLOR, color, 490, 289, 120, 25, clr));

        if (!g_CreateCharacterManager.GetFemale() && g_CreateCharacterManager.GetRace() != RT_ELF)
        {
            entry = (CGUITextEntry *)Add(new CGUITextEntry(
                ID_CCS_FACIAL_HAIR_COLOR,
                toneTextColorRange[0],
                toneTextColorRange[1],
                toneTextColorRange[1],
                490,
                320,
                0,
                false,
                9));
            entry->m_Entry.SetTextA("Facial Hair Color");
            entry->CheckOnSerial = true;
            entry->ReadOnly = true;

            color = g_CreateCharacterManager.BeardColor - 1;

            colorIndex = (color + ((color + (color << 2)) << 1)) << 3;
            colorIndex += (colorIndex / colorOffsetDivider) << 2;
            color = *(u16 *)(huesData + colorIndex);

            clr = g_ColorManager.Color16To32(color);

            Add(new CGUIColoredPolygone(ID_CCS_FACIAL_HAIR_COLOR, color, 490, 333, 120, 25, clr));
        }
    }
    else
    {
        if (g_CreateCharacterScreen.GetColorSelection() == CCSID_SKIN_TONE)
        {
            const u16 *colorPtr = g_CreateCharacterManager.GetSkinTonePtr();
            int maxX = 8;
            int polygoneWidth = 15;
            int maxY = 8;
            int polygoneHeight = 35;

            if (g_CreateCharacterManager.GetRace() == RT_ELF)
            {
                maxX = 4;
                polygoneWidth = 30;
            }
            else if (g_CreateCharacterManager.GetRace() == RT_GARGOYLE)
            {
                maxX = 4;
                polygoneWidth = 30;
                maxY = 7;
                polygoneHeight = 40;
            }

            for (int y = 0; y < maxY; y++)
            {
                for (int x = 0; x < maxX; x++)
                {
                    u16 startColor = *colorPtr++;

                    int colorIndex = (startColor + ((startColor + (startColor << 2)) << 1)) << 3;
                    colorIndex += (colorIndex / colorOffsetDivider) << 2;
                    u16 color = *(u16 *)(huesData + colorIndex);

                    u32 clr = g_ColorManager.Color16To32(color);

                    CGUIColoredPolygone *polygone =
                        (CGUIColoredPolygone *)Add(new CGUIColoredPolygone(
                            ID_CCS_COLOR_RANGE + ((int)x * maxY + (int)y),
                            startColor,
                            491 + ((int)x * polygoneWidth),
                            138 + ((int)y * polygoneHeight),
                            polygoneWidth,
                            polygoneHeight,
                            clr));
                    polygone->DrawDot = true;
                }
            }
        }
        else if (
            g_CreateCharacterScreen.GetColorSelection() == CCSID_SHIRT_COLOR ||
            g_CreateCharacterScreen.GetColorSelection() == CCSID_SKIRT_OR_PANTS_COLOR)
        {
            for (int y = 1; y < 48; y++)
            {
                u16 startColor = (1 << (y % 4)) * 100 + 1 + ((u16)y / 4);

                for (int x = 0; x < 20; x++)
                {
                    int colorIndex = (startColor + ((startColor + (startColor << 2)) << 1)) << 3;
                    colorIndex += (colorIndex / colorOffsetDivider) << 2;
                    u16 color = *(u16 *)(huesData + colorIndex);

                    u32 clr = g_ColorManager.Color16To32(color);

                    CGUIColoredPolygone *polygone =
                        (CGUIColoredPolygone *)Add(new CGUIColoredPolygone(
                            ID_CCS_COLOR_RANGE + ((int)y * 20 + (int)x),
                            startColor,
                            492 + ((int)x * 6),
                            148 + ((int)y * 5),
                            6,
                            5,
                            clr));
                    polygone->DrawDot = true;

                    startColor += 5;
                }
            }
        }
        else if (
            g_CreateCharacterScreen.GetColorSelection() == CCSID_HAIR_COLOR ||
            g_CreateCharacterScreen.GetColorSelection() == CCSID_FACIAL_HAIR_COLOR)
        {
            const u16 *colorPtr = g_CreateCharacterManager.GetHairColorPtr();
            int maxY = 8;
            int polygoneHeight = 35;

            if (g_CreateCharacterManager.GetRace() == RT_ELF)
            {
                maxY = 9;
                polygoneHeight = 31;
            }
            else if (g_CreateCharacterManager.GetRace() == RT_GARGOYLE)
            {
                maxY = 3;
                polygoneHeight = 93;
            }

            for (int y = 0; y < maxY; y++)
            {
                for (int x = 0; x < 6; x++)
                {
                    u16 startColor = *colorPtr++;

                    int colorIndex = (startColor + ((startColor + (startColor << 2)) << 1)) << 3;
                    colorIndex += (colorIndex / colorOffsetDivider) << 2;
                    u16 color = *(u16 *)(huesData + colorIndex);

                    u32 clr = g_ColorManager.Color16To32(color);

                    CGUIColoredPolygone *polygone =
                        (CGUIColoredPolygone *)Add(new CGUIColoredPolygone(
                            ID_CCS_COLOR_RANGE + ((int)x * maxY + (int)y),
                            startColor,
                            490 + ((int)x * 20),
                            140 + ((int)y * polygoneHeight),
                            20,
                            polygoneHeight,
                            clr));
                    polygone->DrawDot = true;
                }
            }
        }
    }

    Add(new CGUIShader(&g_ColorizerShader, true));

    u16 gumpID = g_CreateCharacterManager.GetBodyGump();

    if (gumpID != 0u)
    {
        CGUIGumppic *gumppic = (CGUIGumppic *)Add(new CGUIGumppic(gumpID, 238, 98));
        gumppic->Color = g_CreateCharacterManager.SkinTone;
    }

    gumpID = g_CreateCharacterManager.GetBootsGump();

    if (gumpID != 0u)
    {
        Add(new CGUIGumppic(gumpID, 238, 98));
    }

    gumpID = g_CreateCharacterManager.GetPantsGump();

    if (gumpID != 0u)
    {
        CGUIGumppic *gumppic = (CGUIGumppic *)Add(new CGUIGumppic(gumpID, 238, 98));
        gumppic->Color = g_CreateCharacterManager.PantsColor;
    }

    gumpID = g_CreateCharacterManager.GetShirtGump();

    if (gumpID != 0u)
    {
        CGUIGumppic *gumppic = (CGUIGumppic *)Add(new CGUIGumppic(gumpID, 238, 98));
        gumppic->Color = g_CreateCharacterManager.ShirtColor;
    }

    if (g_CreateCharacterManager.HairStyle != 0u)
    {
        CGUIGumppic *gumppic = (CGUIGumppic *)Add(new CGUIGumppic(
            g_CreateCharacterManager.GetHair(g_CreateCharacterManager.HairStyle).GumpID, 238, 98));
        gumppic->Color = g_CreateCharacterManager.HairColor;
    }

    if (!g_CreateCharacterManager.GetFemale() && (g_CreateCharacterManager.BeardStyle != 0u) &&
        g_CreateCharacterManager.GetRace() != RT_ELF)
    {
        CGUIGumppic *gumppic = (CGUIGumppic *)Add(new CGUIGumppic(
            g_CreateCharacterManager.GetBeard(g_CreateCharacterManager.BeardStyle).GumpID,
            238,
            98));
        gumppic->Color = g_CreateCharacterManager.BeardColor;
    }

    Add(new CGUIShader(nullptr, false));

    if (GameVars::GetClientVersion() < CV_4011D)
    {
        if (g_CreateCharacterManager.GetFemale())
        {
            Add(new CGUIButton(ID_CCS_MALE_BUTTON, 0x070D, 0x070E, 0x070F, 310, 408));
        }
        else
        {
            Add(new CGUIButton(ID_CCS_FEMALE_BUTTON, 0x0710, 0x0711, 0x0712, 310, 408));
        }
    }
    else
    {
        Add(new CGUIGroup(1));

        CGUIRadio *radio =
            (CGUIRadio *)Add(new CGUIRadio(ID_CCS_MALE_BUTTON, 0x0768, 0x0767, 0x0767, 425, 435));
        radio->Checked = !g_CreateCharacterManager.GetFemale();
        radio =
            (CGUIRadio *)Add(new CGUIRadio(ID_CCS_FEMALE_BUTTON, 0x0768, 0x0767, 0x0767, 425, 455));
        radio->Checked = g_CreateCharacterManager.GetFemale();

        Add(new CGUIGroup(0));

        Add(new CGUIButton(ID_CCS_MALE_BUTTON, 0x0710, 0x0711, 0x0712, 445, 435));
        Add(new CGUIButton(ID_CCS_FEMALE_BUTTON, 0x070D, 0x070E, 0x070F, 445, 455));

        Add(new CGUIGroup(2));

        radio = (CGUIRadio *)Add(
            new CGUIRadio(ID_CCS_HUMAN_RACE_BUTTON, 0x0768, 0x0767, 0x0767, 180, 435));
        radio->Checked = (g_CreateCharacterManager.GetRace() == RT_HUMAN);
        radio = (CGUIRadio *)Add(
            new CGUIRadio(ID_CCS_ELF_RACE_BUTTON, 0x0768, 0x0767, 0x0767, 180, 455));
        radio->Checked = (g_CreateCharacterManager.GetRace() == RT_ELF);

        Add(new CGUIButton(ID_CCS_HUMAN_RACE_BUTTON, 0x0702, 0x0703, 0x0704, 200, 435));
        Add(new CGUIButton(ID_CCS_ELF_RACE_BUTTON, 0x0705, 0x0706, 0x0707, 200, 455));

        if (GameVars::GetClientVersion() >= CV_60144)
        {
            radio = (CGUIRadio *)Add(
                new CGUIRadio(ID_CCS_GARGOYLE_RACE_BUTTON, 0x0768, 0x0767, 0x0767, 60, 435));
            radio->Checked = (g_CreateCharacterManager.GetRace() == RT_GARGOYLE);

            Add(new CGUIButton(ID_CCS_GARGOYLE_RACE_BUTTON, 0x07D3, 0x07D4, 0x07D5, 80, 435));
        }

        Add(new CGUIGroup(0));
    }
}

void CGumpScreenCreateCharacter::InitToolTip()
{
    if (!g_ConfigManager.UseToolTips || g_SelectedObject.Object == nullptr)
    {
        return;
    }

    u32 id = g_SelectedObject.Serial;

    switch (id)
    {
        case ID_CCS_QUIT:
        {
            g_ToolTip.Set(L"Quit Ultima Online", 80);
            break;
        }
        case ID_CCS_ARROW_NEXT:
        {
            g_ToolTip.Set(L"Accept this character");
            break;
        }
        case ID_CCS_ARROW_PREV:
        {
            g_ToolTip.Set(L"Back to character generation screen", 150);
            break;
        }
        default:
            break;
    }
}

void CGumpScreenCreateCharacter::GUMP_BUTTON_EVENT_C
{
    if (serial == ID_CCS_QUIT)
    { //x button
        g_CreateCharacterScreen.CreateSmoothAction(CCreateCharacterScreen::ID_SMOOTH_CCS_QUIT);
    }
    else if (serial == ID_CCS_ARROW_PREV)
    { //< button
        g_CreateCharacterScreen.CreateSmoothAction(
            CCreateCharacterScreen::ID_SMOOTH_CCS_GO_SCREEN_CHARACTER);
    }
    else if (serial == ID_CCS_ARROW_NEXT) //> button
    {
        if (g_EntryPointer->Length() < 2)
        {
            g_CreateCharacterScreen.CreateSmoothAction(
                CCreateCharacterScreen::ID_SMOOTH_CCS_GO_SCREEN_CONNECT);
        }
        else
        {
            g_CreateCharacterScreen.CreateSmoothAction(
                CCreateCharacterScreen::ID_SMOOTH_CCS_GO_SCREEN_SELECT_TOWN);
        }
    }
    else if (serial == ID_CCS_FEMALE_BUTTON)
    {
        g_CreateCharacterManager.SetFemale(true);
        WantUpdateContent = true;
        g_CreateCharacterScreen.SetColorSelection(0);
    }
    else if (serial == ID_CCS_MALE_BUTTON)
    {
        g_CreateCharacterManager.SetFemale(false);
        WantUpdateContent = true;
        g_CreateCharacterScreen.SetColorSelection(0);
    }
    else if (serial == ID_CCS_HUMAN_RACE_BUTTON)
    {
        g_CreateCharacterManager.SetRace(RT_HUMAN);
        WantUpdateContent = true;
        g_CreateCharacterScreen.SetColorSelection(0);
    }
    else if (serial == ID_CCS_ELF_RACE_BUTTON)
    {
        g_CreateCharacterManager.SetRace(RT_ELF);
        WantUpdateContent = true;
        g_CreateCharacterScreen.SetColorSelection(0);
    }
    else if (serial == ID_CCS_GARGOYLE_RACE_BUTTON)
    {
        g_CreateCharacterManager.SetRace(RT_GARGOYLE);
        WantUpdateContent = true;
        g_CreateCharacterScreen.SetColorSelection(0);
    }
}

void CGumpScreenCreateCharacter::GUMP_RADIO_EVENT_C
{
    if (state)
    {
        if (serial == ID_CCS_MALE_BUTTON)
        {
            g_CreateCharacterManager.SetFemale(false);
        }
        else if (serial == ID_CCS_FEMALE_BUTTON)
        {
            g_CreateCharacterManager.SetFemale(true);
        }
        else if (serial == ID_CCS_HUMAN_RACE_BUTTON)
        {
            g_CreateCharacterManager.SetRace(RT_HUMAN);
        }
        else if (serial == ID_CCS_ELF_RACE_BUTTON)
        {
            g_CreateCharacterManager.SetRace(RT_ELF);
        }
        else if (serial == ID_CCS_GARGOYLE_RACE_BUTTON)
        {
            g_CreateCharacterManager.SetRace(RT_GARGOYLE);
        }

        WantUpdateContent = true;
        g_CreateCharacterScreen.SetColorSelection(0);
    }
}

void CGumpScreenCreateCharacter::GUMP_TEXT_ENTRY_EVENT_C
{
    if (g_CreateCharacterScreen.GetColorSelection() == 0)
    {
        if (serial == ID_CCS_SKIN_TONE)
        {
            g_CreateCharacterScreen.SetColorSelection(CCSID_SKIN_TONE);
        }
        else if (serial == ID_CCS_SHIRT_COLOR)
        {
            g_CreateCharacterScreen.SetColorSelection(CCSID_SHIRT_COLOR);
        }
        else if (serial == ID_CCS_SKIRT_OR_PANTS_COLOR)
        {
            g_CreateCharacterScreen.SetColorSelection(CCSID_SKIRT_OR_PANTS_COLOR);
        }
        else if (serial == ID_CCS_HAIR_COLOR)
        {
            g_CreateCharacterScreen.SetColorSelection(CCSID_HAIR_COLOR);
        }
        else if (!g_CreateCharacterManager.GetFemale() && serial == ID_CCS_FACIAL_HAIR_COLOR)
        {
            g_CreateCharacterScreen.SetColorSelection(CCSID_FACIAL_HAIR_COLOR);
        }

        WantUpdateContent = true;
    }
    else
    {
        if (serial >= ID_CCS_COLOR_RANGE)
        {
            u16 color = g_SelectedObject.Object->Color + 1;

            if (g_CreateCharacterScreen.GetColorSelection() == CCSID_SKIN_TONE)
            {
                if (g_CreateCharacterManager.GetRace() == RT_HUMAN)
                {
                    if (color < 0x03EA)
                    {
                        color = 0x03EA;
                    }
                    else if (color > 0x0422)
                    {
                        color = 0x0422;
                    }
                }

                g_CreateCharacterManager.SkinTone = color;
            }
            else if (
                g_CreateCharacterScreen.GetColorSelection() == CCSID_SHIRT_COLOR ||
                g_CreateCharacterScreen.GetColorSelection() == CCSID_SKIRT_OR_PANTS_COLOR)
            {
                if (g_CreateCharacterScreen.GetColorSelection() == CCSID_SHIRT_COLOR)
                {
                    g_CreateCharacterManager.ShirtColor = color;
                }
                else
                {
                    g_CreateCharacterManager.PantsColor = color;
                }
            }
            else if (
                g_CreateCharacterScreen.GetColorSelection() == CCSID_HAIR_COLOR ||
                g_CreateCharacterScreen.GetColorSelection() == CCSID_FACIAL_HAIR_COLOR)
            {
                if (g_CreateCharacterManager.GetRace() == RT_HUMAN)
                {
                    if (color < 0x044E)
                    {
                        color = 0x044E;
                    }
                    else if (color > 0x047C)
                    {
                        color = 0x047C;
                    }
                }

                if (g_CreateCharacterScreen.GetColorSelection() == CCSID_HAIR_COLOR)
                {
                    g_CreateCharacterManager.HairColor = color;
                }
                else
                {
                    g_CreateCharacterManager.BeardColor = color;
                }
            }

            g_CreateCharacterScreen.SetColorSelection(0);
            WantUpdateContent = true;
        }
    }
}

void CGumpScreenCreateCharacter::GUMP_COMBOBOX_SELECTION_EVENT_C
{
    if (serial >= ID_CCS_HAIR_STYLE)
    {
        if (serial >= ID_CCS_FACIAL_HAIR_STYLE)
        {
            u8 index = serial - ID_CCS_FACIAL_HAIR_STYLE;

            if (index < g_CreateCharacterManager.GetCurrentFacialHairCount())
            {
                g_CreateCharacterManager.BeardStyle = (u8)index;
            }

            WantUpdateContent = true;
        }
        else
        {
            u8 index = serial - ID_CCS_HAIR_STYLE;

            if (index < g_CreateCharacterManager.GetCurrentHairCount())
            {
                g_CreateCharacterManager.HairStyle = index;
            }

            WantUpdateContent = true;
        }
    }
}
