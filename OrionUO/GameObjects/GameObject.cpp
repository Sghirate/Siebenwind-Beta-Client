#include "GameObject.h"
#include "Core/StringUtils.h"
#include "GameEffect.h"
#include "GameCharacter.h"
#include "GameVars.h"
#include "GameWindow.h"
#include "Globals.h"
#include "SiebenwindClient.h"
#include <SDL_timer.h>
#include "Config.h"
#include "OrionUO.h"
#include "ServerList.h"
#include "SelectedObject.h"
#include "Managers/AnimationManager.h"
#include "Managers/GumpManager.h"
#include "Managers/ClilocManager.h"
#include "Managers/MouseManager.h"
#include "Managers/FontsManager.h"
#include "Managers/ConfigManager.h"
#include "Managers/ColorManager.h"
#include "TextEngine/TextData.h"
#include "plugin/enumlist.h"

CGameObject::CGameObject(int serial)
    : CRenderStaticObject(ROT_GAME_OBJECT, serial, 0, 0, 0, 0, 0)
    , LastAnimationChangeTime(SDL_GetTicks())
{
    memset(&m_FrameInfo, 0, sizeof(DRAW_FRAME_INFORMATION));

#if UO_DEBUG_INFO != 0
    g_GameObjectsCount++;
#endif //UO_DEBUG_INFO!=0
}

CGameObject::~CGameObject()
{
    if (m_Effects != nullptr)
    {
        delete m_Effects;
        m_Effects = nullptr;
    }

    m_Next = nullptr;
    m_Prev = nullptr;

    if (m_TextureObjectHalndes.Texture != 0)
    {
        glDeleteTextures(1, &m_TextureObjectHalndes.Texture);
        m_TextureObjectHalndes.Texture = 0;
    }

    Clear();

#if UO_DEBUG_INFO != 0
    g_GameObjectsCount--;
#endif //UO_DEBUG_INFO!=0
}

void CGameObject::SetFlags(u8 val)
{
    bool poisoned = Poisoned();
    bool yellowHits = YellowHits();

    m_Flags = val;

    if (poisoned != Poisoned() || yellowHits != YellowHits())
    {
        g_GumpManager.UpdateContent(Serial, 0, GT_STATUSBAR);
        g_GumpManager.UpdateContent(Serial, 0, GT_TARGET_SYSTEM);
    }
}

void CGameObject::SetName(const std::string &newName)
{
    if (IsPlayer() && m_Name != newName)
    {
        if (g_GameState >= GS_GAME)
        {
            std::string title = SiebenwindClient::GetWindowTitle() + " - " + newName;

            CServer *server = g_ServerList.GetSelectedServer();

            if (server != nullptr)
            {
                title += " (" + server->Name + ")";
            }

            g_gameWindow.SetTitle(title.c_str()); // FIXME: remove this dependency from here
        }
    }

    m_Name = newName;
}

void CGameObject::DrawObjectHandlesTexture()
{
    if (m_TextureObjectHalndes.Texture == 0)
    {
        if (NPC || IsCorpse())
        {
            GenerateObjectHandlesTexture(Core::ToWString(m_Name));
        }
        else
        {
            std::wstring name = Core::ToWString(m_Name);
            if (name.length() == 0u)
            {
                name = g_ClilocManager.GetCliloc(g_Language)
                           ->GetW(1020000 + Graphic, true, g_Orion.m_StaticData[Graphic].Name);
            }
            GenerateObjectHandlesTexture(name);
        }
    }

    int x = DrawX - g_ObjectHandlesWidthOffset;
    int y = DrawY;

    if (NPC)
    {
        CGameCharacter *gc = (CGameCharacter *)this;

        ANIMATION_DIMENSIONS dims = g_AnimationManager.GetAnimationDimensions(this);

        x += gc->OffsetX;
        y += gc->OffsetY - (gc->OffsetZ + dims.Height + dims.CenterY + 8);
    }
    else
    {
        y -= g_Orion.GetStaticArtDimension(Graphic).y;
    }

    m_TextureObjectHalndes.Draw(x, y);
}

void CGameObject::SelectObjectHandlesTexture()
{
    if (m_TextureObjectHalndes.Texture != 0)
    {
        int x = DrawX - g_ObjectHandlesWidthOffset;
        int y = DrawY;

        if (NPC)
        {
            CGameCharacter *gc = (CGameCharacter *)this;

            ANIMATION_DIMENSIONS dims = g_AnimationManager.GetAnimationDimensions(this);

            x += gc->OffsetX;
            y += gc->OffsetY - (gc->OffsetZ + dims.Height + dims.CenterY + 8);
        }
        else
        {
            y -= g_Orion.GetStaticArtDimension(Graphic).y;
        }

        Core::TMousePos pos = g_MouseManager.GetPosition();
        x = pos.x - x;
        y = pos.y - y;

        if (x < 0 || x >= g_ObjectHandlesWidth || y < 0 || y >= g_ObjectHandlesHeight)
        {
            return;
        }

        if (g_ObjectHandlesBackgroundPixels[(y * g_ObjectHandlesWidth) + x] != 0)
        {
            g_SelectedObject.Init(this);
            g_SelectedGameObjectHandle = Serial;
        }
    }
}

void CGameObject::GenerateObjectHandlesTexture(std::wstring text)
{
    if (m_TextureObjectHalndes.Texture != 0)
    {
        glDeleteTextures(1, &m_TextureObjectHalndes.Texture);
        m_TextureObjectHalndes.Texture = 0;
    }

    int width = g_ObjectHandlesWidth - 20;

    u8 font = 1;
    CGLTextTexture textTexture;
    u16 color = 0xFFFF;
    u8 cell = 30;
    TEXT_ALIGN_TYPE tat = TS_CENTER;
    u16 flags = 0;

    if (g_FontManager.GetWidthW(font, text) > width)
    {
        text = g_FontManager.GetTextByWidthW(font, text, width - 6, true);
    }

    std::vector<u32> textData = g_FontManager.GeneratePixelsW(
        font, textTexture, text.c_str(), color, cell, width, tat, flags);

    if (textData.empty())
    {
        return;
    }

    static const int size = g_ObjectHandlesWidth * g_ObjectHandlesHeight;
    u16 pixels[size] = { 0 };

    memcpy(&pixels[0], &g_ObjectHandlesBackgroundPixels[0], size * 2);

    color = 0;

    if (NPC)
    {
        if (IsPlayer())
        {
            color = 0x0386;
        }
        else
        {
            color = g_ConfigManager.GetColorByNotoriety(GameCharacterPtr()->Notoriety);
        }

        if (color != 0u)
        {
            for (int x = 0; x < g_ObjectHandlesWidth; x++)
            {
                for (int y = 0; y < g_ObjectHandlesHeight; y++)
                {
                    u16 &pixel = pixels[(y * g_ObjectHandlesWidth) + x];

                    if (pixel != 0u)
                    {
                        u8 r = (pixel & 0x1F);
                        u8 g = ((pixel >> 5) & 0x1F);
                        u8 b = ((pixel >> 10) & 0x1F);

                        if (r == g && r == b)
                        {
                            pixel = g_ColorManager.GetColor16(pixel, color) | 0x8000;
                        }
                    }
                }
            }
        }
    }

    int maxHeight = textTexture.Height;

    for (int x = 0; x < width; x++)
    {
        int gumpDataX = (int)x + 10;

        for (int y = 0; y < maxHeight; y++)
        {
            int gumpDataY = (int)y + 1;

            if (gumpDataY >= g_ObjectHandlesHeight)
            {
                break;
            }

            u32 &pixel = textData[(y * textTexture.Width) + x];

            if (pixel != 0u)
            {
                u8 *bytes = (u8 *)&pixel;
                u8 buf = bytes[0];
                bytes[0] = bytes[3];
                bytes[3] = buf;
                buf = bytes[1];
                bytes[1] = bytes[2];
                bytes[2] = buf;
                pixels[(gumpDataY * g_ObjectHandlesWidth) + gumpDataX] =
                    g_ColorManager.Color32To16(pixel) | 0x8000;
            }
        }
    }

    g_GL_BindTexture16(m_TextureObjectHalndes, g_ObjectHandlesWidth, g_ObjectHandlesHeight, pixels);
}

void CGameObject::AddText(CTextData *msg)
{

    msg->Owner = this;
    m_TextControl->Add(msg);

    if (Container == 0xFFFFFFFF)
    {
        Changed = true;
    }
    else
    {
        UpdateTextCoordinates();
        FixTextCoordinates();
    }

    /*if (m_Clicked)
	{
		m_Clicked = false;

		if (IsPlayer()) //(NPC)
			msgname = m_Name + ": ";
	}*/

    g_Orion.AddJournalMessage(msg, JournalPrefix);
}

u16 CGameObject::GetMountAnimation()
{
    return Graphic; // + UO->GetStaticPointer(Graphic)->Increment;
}

void CGameObject::Clear()
{
    if (!Empty())
    {
        CGameObject *obj = (CGameObject *)m_Items;

        while (obj != nullptr)
        {
            CGameObject *next = (CGameObject *)obj->m_Next;

            g_World->RemoveObject(obj);

            obj = next;
        }

        m_Items = nullptr;
    }
}

void CGameObject::ClearUnequipped()
{
    if (!Empty())
    {
        CGameObject *newFirstItem = nullptr;
        CGameObject *obj = (CGameObject *)m_Items;

        while (obj != nullptr)
        {
            CGameObject *next = (CGameObject *)obj->m_Next;

            if (((CGameItem *)obj)->Layer != OL_NONE)
            {
                if (newFirstItem == nullptr)
                {
                    newFirstItem = obj;
                }
            }
            else
            {
                g_World->RemoveObject(obj);
            }

            obj = next;
        }

        m_Items = newFirstItem;
    }
}

void CGameObject::ClearNotOpenedItems()
{
    if (!Empty())
    {
        CGameObject *obj = (CGameObject *)m_Items;

        while (obj != nullptr)
        {
            CGameObject *next = (CGameObject *)obj->m_Next;

            if (!obj->NPC && !((CGameItem *)obj)->Opened &&
                ((CGameItem *)obj)->Layer != OL_BACKPACK)
            {
                g_World->RemoveObject(obj);
            }

            obj = next;
        }
    }
}

bool CGameObject::Poisoned()
{
    if (GameVars::GetClientVersion() >= CV_7000)
    {
        return SA_Poisoned;
    }
    return (m_Flags & 0x04) != 0;
}

bool CGameObject::Flying()
{
    if (GameVars::GetClientVersion() >= CV_7000)
    {
        return (m_Flags & 0x04) != 0;
    }
    return false;
}

int CGameObject::IsGold(u16 graphic)
{
    switch (graphic)
    {
        case 0x0EED:
            return 1;
        /*case 0x0EEA:
            return 2;*/
        case 0x0EF0:
            return 3;
        default:
            break;
    }

    return 0;
}

u16 CGameObject::GetDrawGraphic(bool &doubleDraw)
{
    int index = IsGold(Graphic);
    u16 result = Graphic;

    const u16 graphicAssociateTable[3][3] = { { 0x0EED, 0x0EEE, 0x0EEF },
                                                   { 0x0EEA, 0x0EEB, 0x0EEC },
                                                   { 0x0EF0, 0x0EF1, 0x0EF2 } };

    if (index != 0)
    {
        int graphicIndex = (int)(Count > 1) + (int)(Count > 5);
        result = graphicAssociateTable[index - 1][graphicIndex];
    }
    else
    {
        doubleDraw = IsStackable() && (Count > 1);
    }

    return result;
}

void CGameObject::DrawEffects(int x, int y)
{
    if (NPC)
    {
        CGameCharacter *gc = GameCharacterPtr();

        x += gc->OffsetX;
        y += gc->OffsetY - (int)gc->OffsetZ - 3;
    }

    QFOR(effect, m_Effects, CGameEffect *)
    {
        effect->ApplyRenderMode();

        if (effect->EffectType == EF_LIGHTING)
        {
            u16 graphic = 0x4E20 + effect->AnimIndex;

            Core::Vec2<i32> size = g_Orion.GetGumpDimension(graphic);

            g_Orion.DrawGump(graphic, effect->Color, x - (size.x / 2), y - size.y);
        }
        else
        {
            g_Orion.DrawStaticArt(effect->GetCurrentGraphic(), effect->Color, x, y);
        }

        effect->RemoveRenderMode();
    }
}

void CGameObject::UpdateEffects()
{
    CGameEffect *effect = m_Effects;

    while (effect != nullptr)
    {
        CGameEffect *next = (CGameEffect *)effect->m_Next;

        effect->Update(this);

        effect = next;
    }
}

void CGameObject::AddEffect(CGameEffect *effect)
{
    if (m_Effects == nullptr)
    {
        m_Effects = effect;
        effect->m_Next = nullptr;
        effect->m_Prev = nullptr;
    }
    else
    {
        effect->m_Next = m_Effects;
        m_Effects->m_Prev = effect;
        effect->m_Prev = nullptr;
        m_Effects = effect;
    }
}

void CGameObject::RemoveEffect(CGameEffect *effect)
{
    if (effect->m_Prev == nullptr)
    {
        m_Effects = (CGameEffect *)effect->m_Next;

        if (m_Effects != nullptr)
        {
            m_Effects->m_Prev = nullptr;
        }
    }
    else
    {
        effect->m_Prev->m_Next = effect->m_Next;

        if (effect->m_Next != nullptr)
        {
            effect->m_Next->m_Prev = effect->m_Prev;
        }
    }

    effect->m_Next = nullptr;
    effect->m_Prev = nullptr;
    delete effect;
}

void CGameObject::AddObject(CGameObject *obj)
{
    g_World->RemoveFromContainer(obj);

    if (m_Next == nullptr)
    {
        m_Next = obj;
        m_Next->m_Prev = this;
        m_Next->m_Next = nullptr;

        ((CGameObject *)m_Next)->Container = Container;
    }
    else
    {
        CGameObject *item = (CGameObject *)m_Next;

        while (item->m_Next != nullptr)
        {
            item = (CGameObject *)item->m_Next;
        }

        item->m_Next = obj;
        obj->m_Next = nullptr;
        obj->m_Prev = item;

        obj->Container = Container;
    }
}

void CGameObject::AddItem(CGameObject *obj)
{
    if (obj->Container != 0xFFFFFFFF)
    {
        return;
    }

    g_World->RemoveFromContainer(obj);

    if (m_Items != nullptr)
    {
        CGameObject *item = (CGameObject *)Last();

        item->m_Next = obj;
        obj->m_Next = nullptr;
        obj->m_Prev = item;
    }
    else
    {
        m_Items = obj;
        m_Items->m_Next = nullptr;
        m_Items->m_Prev = nullptr;
    }

    obj->Container = Serial;
}

void CGameObject::Reject(CGameObject *obj)
{
    if (obj->Container != Serial)
    {
        return;
    }

    if (m_Items != nullptr)
    {
        if (((CGameObject *)m_Items)->Serial == obj->Serial)
        {
            if (m_Items->m_Next != nullptr)
            {
                m_Items = m_Items->m_Next;
                m_Items->m_Prev = nullptr;
            }
            else
            {
                m_Items = nullptr;
            }
        }
        else
        {
            if (obj->m_Next != nullptr)
            {
                if (obj->m_Prev != nullptr)
                {
                    obj->m_Prev->m_Next = obj->m_Next;
                    obj->m_Next->m_Prev = obj->m_Prev;
                }
                else
                { //WTF???
                    obj->m_Next->m_Prev = nullptr;
                }
            }
            else if (obj->m_Prev != nullptr)
            {
                obj->m_Prev->m_Next = nullptr;
            }
        }
    }

    obj->m_Next = nullptr;
    obj->m_Prev = nullptr;
    obj->Container = 0xFFFFFFFF;
}

CGameObject *CGameObject::GetTopObject()
{
    CGameObject *obj = this;

    while (obj->Container != 0xFFFFFFFF)
    {
        obj = g_World->FindWorldObject(obj->Container);
    }
    return obj;
}

CGameItem *CGameObject::FindLayer(int layer)
{
    QFOR(obj, m_Items, CGameItem *)
    {
        if (obj->Layer == layer)
        {
            return obj;
        }
    }

    return nullptr;
}

bool CGameObject::Caller()
{
    if (GameVars::GetClientVersion() >= CV_7000)
    {
        return pvpCaller;
    }
    return false;
}
