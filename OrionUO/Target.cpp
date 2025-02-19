#include "Target.h"
#include "GameVars.h"
#include "Globals.h"
#include "Config.h"
#include "OrionUO.h"
#include "Multi.h"
#include "plugin/mulstruct.h"
#include "Network/Packets.h"
#include "Managers/FileManager.h"
#include "Managers/MouseManager.h"
#include "Managers/MapManager.h"
#include "Managers/CustomHousesManager.h"
#include "GameObjects/GameCharacter.h"
#include "GameObjects/CustomHouseMultiObject.h"
#include "Gumps/GumpCustomHouse.h"

CTarget g_Target;

CTarget::CTarget()
{
    //Чистимся
    memset(m_Data, 0, sizeof(m_Data));
    memset(m_LastData, 0, sizeof(m_LastData));
}

void CTarget::Reset()
{
    //Чистимся
    memset(m_Data, 0, sizeof(m_Data));
    memset(m_LastData, 0, sizeof(m_LastData));

    if (m_Multi != nullptr)
    {
        delete m_Multi;
        m_Multi = nullptr;
    }

    Type         = 0;
    CursorType   = 0;
    CursorID     = 0;
    Targeting    = false;
    MultiGraphic = 0;
}

void CTarget::RequestFromCustomHouse()
{
    Type         = 2;
    CursorID     = 0;
    CursorType   = 0;
    Targeting    = true;
    MultiGraphic = 0;

    if (g_CustomHouseGump != nullptr)
    {
        g_CustomHouseGump->Erasing           = false;
        g_CustomHouseGump->SeekTile          = false;
        g_CustomHouseGump->SelectedGraphic   = 0;
        g_CustomHouseGump->CombinedStair     = false;
        g_CustomHouseGump->WantUpdateContent = true;
    }
}

void CTarget::SetLastTargetObject(int serial)
{
    Type = 0;
    pack32(m_LastData + 7, serial);
}

void CTarget::SetData(Core::StreamReader& reader)
{
    memcpy(&m_Data[0], reader.GetBuffer(), reader.GetSize());

    Type         = reader.ReadLE<u8>();
    CursorID     = reader.ReadBE<u32>();
    CursorType   = reader.ReadLE<u8>();
    Targeting    = (CursorType < 3);
    MultiGraphic = 0;
}

void CTarget::SetMultiData(Core::StreamReader& reader)
{
    Type       = 1;
    CursorType = 0;
    Targeting  = true;
    CursorID   = reader.ReadBE<u32>(1);

    memset(&m_Data[0], 0, 19);
    m_Data[0] = 0x6C;
    m_Data[1] = 1;                                 //Таргет на ландшафт
    memcpy(m_Data + 2, reader.GetBuffer() + 2, 4); //Копируем ID курсора (ID дида)

    reader.ResetPtr();
    reader.Move(18);
    MultiGraphic = reader.ReadBE<u16>() + 1;
    MultiX       = reader.ReadBE<u16>();
    MultiY       = reader.ReadBE<u16>();
}

void CTarget::SendTargetObject(int serial)
{
    if (!Targeting)
    {
        return; //Если в клиенте нет таргета - выход
    }

    //Пишем серийник объекта, на который ткнули прицелом, остальное - затираем
    pack32(m_Data + 7, serial);
    m_Data[1] = 0;

    CGameObject* obj = (g_World != nullptr ? g_World->FindWorldObject(serial) : nullptr);

    if (obj != nullptr)
    {
        pack16(m_Data + 11, obj->GetX());
        pack16(m_Data + 13, obj->GetY());
        m_Data[15] = 0xFF;
        m_Data[16] = obj->GetZ();
        pack16(m_Data + 17, obj->Graphic);
    }
    else
    {
        pack32(m_Data + 11, 0);
        pack32(m_Data + 15, 0);
    }

    if (serial != g_PlayerSerial)
    {
        g_LastTargetObject = serial;

        //Скопируем для LastTarget
        memcpy(m_LastData, m_Data, sizeof(m_Data));

        if (obj != nullptr && obj->NPC && ((CGameCharacter*)obj)->MaxHits == 0)
        {
            CPacketStatusRequest(serial).Send();
        }
    }

    SendTarget();
}

void CTarget::SendTargetTile(u16 tileID, short x, short y, char z)
{
    if (!Targeting)
    {
        return; //Если в клиенте нет таргета - выход
    }

    m_Data[1] = 1;

    //Пишем координаты и индекс тайла, на который ткнули, остальное трем
    pack32(m_Data + 7, 0);
    pack16(m_Data + 11, x);
    pack16(m_Data + 13, y);

    //m_Data[15] = 0xFF;
    //m_Data[16] = z;
    pack16(m_Data + 15, (short)z);
    pack16(m_Data + 17, tileID);

    //Скопируем для LastTarget
    memcpy(m_LastData, m_Data, sizeof(m_Data));

    SendTarget();
}

void CTarget::SendCancelTarget()
{
    if (!Targeting)
    {
        return; //Если в клиенте нет таргета - выход
    }

    //Уходят только нули
    pack32(m_Data + 7, 0);
    pack32(m_Data + 11, 0xFFFFFFFF);
    pack32(m_Data + 15, 0);

    SendTarget();

    if (g_CustomHouseGump != nullptr)
    {
        g_CustomHouseGump->Erasing           = false;
        g_CustomHouseGump->SeekTile          = false;
        g_CustomHouseGump->SelectedGraphic   = 0;
        g_CustomHouseGump->CombinedStair     = false;
        g_CustomHouseGump->WantUpdateContent = true;
    }
}

void CTarget::Plugin_SendTargetObject(int serial)
{
    if (!Targeting)
    {
        return; //Если в клиенте нет таргета - выход
    }

    //Пишем серийник объекта, на который ткнули прицелом, остальное - затираем
    pack32(m_Data + 7, serial);
    m_Data[1] = 0;

    CGameObject* obj = (g_World != nullptr ? g_World->FindWorldObject(serial) : nullptr);

    if (obj != nullptr)
    {
        pack16(m_Data + 11, obj->GetX());
        pack16(m_Data + 13, obj->GetY());
        m_Data[15] = 0xFF;
        m_Data[16] = obj->GetZ();
        pack16(m_Data + 17, obj->Graphic);
    }
    else
    {
        pack32(m_Data + 11, 0);
        pack32(m_Data + 15, 0);
    }

    if (serial != g_PlayerSerial)
    {
        g_LastTargetObject = serial;

        //Скопируем для LastTarget
        memcpy(m_LastData, m_Data, sizeof(m_Data));

        if (obj != nullptr && obj->NPC && ((CGameCharacter*)obj)->MaxHits == 0)
        {
            CPacketStatusRequest(serial).Send();
        }
    }

    Plugin_SendTarget();
}

void CTarget::Plugin_SendTargetTile(u16 tileID, short x, short y, char z)
{
    if (!Targeting)
    {
        return; //Если в клиенте нет таргета - выход
    }

    m_Data[1] = 1;

    //Пишем координаты и индекс тайла, на который ткнули, остальное трем
    pack32(m_Data + 7, 0);
    pack16(m_Data + 11, x);
    pack16(m_Data + 13, y);

    //m_Data[15] = 0xFF;
    //m_Data[16] = z;
    pack16(m_Data + 15, (short)z);
    pack16(m_Data + 17, tileID);

    //Скопируем для LastTarget
    memcpy(m_LastData, m_Data, sizeof(m_Data));

    Plugin_SendTarget();
}

void CTarget::Plugin_SendCancelTarget()
{
    if (!Targeting)
    {
        return; //Если в клиенте нет таргета - выход
    }

    //Уходят только нули
    pack32(m_Data + 7, 0);
    pack32(m_Data + 11, 0xFFFFFFFF);
    pack32(m_Data + 15, 0);

    Plugin_SendTarget();
}

void CTarget::SendLastTarget()
{
    if (!Targeting)
    {
        return; //Если в клиенте нет таргета - выход
    }

    //Восстановим пакет последнего актуального таргета
    memcpy(m_Data, m_LastData, sizeof(m_Data));
    m_Data[0] = 0x6C;
    m_Data[1] = Type;
    m_Data[6] = CursorType;
    pack32(m_Data + 2, CursorID);

    SendTarget();
}

void CTarget::SendTarget()
{
    if (Type != 2)
    {
        g_Orion.Send(m_Data, sizeof(m_Data));
    }

    memset(m_Data, 0, sizeof(m_Data));
    Targeting    = false;
    MultiGraphic = 0;

    g_MouseManager.CancelDoubleClick = true;
}

void CTarget::Plugin_SendTarget()
{
    // TODO: reimplement: UOMsg_Send(m_Data, sizeof(m_Data));
    memset(m_Data, 0, sizeof(m_Data));
    Targeting    = false;
    MultiGraphic = 0;

    g_MouseManager.CancelDoubleClick = true;
}

void CTarget::UnloadMulti()
{
    if (m_Multi != nullptr)
    {
        delete m_Multi;
        m_Multi = nullptr;
    }
}

void CTarget::LoadMulti(int offsetX, int offsetY, char offsetZ)
{
    UnloadMulti();

    CIndexMulti& index = g_Orion.m_MultiDataIndex[MultiGraphic - 1];

    int count = (int)index.Count;

    if (index.UopBlock != nullptr)
    {
        std::vector<u8> data = g_FileManager.m_MultiCollection.GetData(*index.UopBlock);

        if (data.empty())
        {
            return;
        }

        Core::StreamReader reader(&data[0], data.size());
        reader.Move(8); //ID + Count

        for (int i = 0; i < count; i++)
        {
            u16 graphic      = reader.ReadLE<u16>();
            short x          = reader.ReadLE<i16>();
            short y          = reader.ReadLE<i16>();
            short z          = reader.ReadLE<i16>();
            u16 flags        = reader.ReadLE<u16>();
            u32 clilocsCount = reader.ReadLE<u32>();

            if (clilocsCount != 0u)
            {
                reader.Move(clilocsCount * 4);
            }

            CMultiObject* mo =
                new CMultiObject(graphic, x + offsetX, y + offsetY, (char)z + (char)offsetZ, 2);
            g_MapManager.AddRender(mo);
            AddMultiObject(mo);
        }
    }
    else if (index.Address != 0)
    {
        int itemOffset = sizeof(MULTI_BLOCK);

        if (GameVars::GetClientVersion() >= CV_7090)
        {
            itemOffset = sizeof(MULTI_BLOCK_NEW);
        }

        for (int j = 0; j < count; j++)
        {
            MULTI_BLOCK* pmb = (MULTI_BLOCK*)(index.Address + (j * itemOffset));

            CMultiObject* mo = new CMultiObject(
                pmb->ID, offsetX + pmb->X, offsetY + pmb->Y, offsetZ + (char)pmb->Z, 2);
            g_MapManager.AddRender(mo);
            AddMultiObject(mo);
        }
    }
}

void CTarget::AddMultiObject(CMultiObject* obj)
{
    if (m_Multi == nullptr)
    {
        m_Multi          = new CMulti(obj->GetX(), obj->GetY());
        m_Multi->m_Next  = nullptr;
        m_Multi->m_Prev  = nullptr;
        m_Multi->m_Items = obj;
        obj->m_Next      = nullptr;
        obj->m_Prev      = nullptr;
    }
    else
    {
        CMulti* multi = GetMultiAtXY(obj->GetX(), obj->GetY());

        if (multi != nullptr)
        {
            QFOR(multiobj, multi->m_Items, CMultiObject*)
            {
                if (obj->GetZ() < multiobj->GetZ())
                {
                    if (multiobj->m_Prev == nullptr)
                    {
                        multi->Insert(multiobj->m_Prev, obj);
                    }
                    else
                    {
                        multi->Insert(multiobj, obj);
                    }

                    return;
                }

                if (multiobj->m_Next == nullptr)
                {
                    multiobj->m_Next = obj;
                    obj->m_Prev      = multiobj;
                    obj->m_Next      = nullptr;

                    return;
                }
            }

            //Если пришли сюда - что-то пошло не так
        }
        else
        {
            // FIXME: potential leak, validate
            CMulti* newmulti  = new CMulti(obj->GetX(), obj->GetY());
            newmulti->m_Next  = nullptr;
            newmulti->m_Items = obj;
            obj->m_Next       = nullptr;
            obj->m_Prev       = nullptr;

            multi = m_Multi;

            while (multi != nullptr)
            {
                if (multi->m_Next == nullptr)
                {
                    multi->m_Next    = newmulti;
                    newmulti->m_Prev = multi;
                    break;
                }

                multi = (CMulti*)multi->m_Next;
            }
        }
    }
}

CMulti* CTarget::GetMultiAtXY(short x, short y)
{
    CMulti* multi = m_Multi;

    while (multi != nullptr)
    {
        if (multi->X == x && multi->Y == y)
        {
            break;
        }

        multi = (CMulti*)multi->m_Next;
    }

    return multi;
}
