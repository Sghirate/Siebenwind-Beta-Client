// MIT License
// Copyright (C) August 2016 Hotride

#include "MultiMap.h"
#include "Core/MappedFile.h"
#include "Logging.h"
#include "Gumps/GumpMap.h"
#include "Managers/ColorManager.h"
#include "Managers/FileManager.h"

CMultiMap g_MultiMap;

CMultiMap::CMultiMap()
{
}

CMultiMap::~CMultiMap()
{
}

void CMultiMap::LoadMap(CGumpMap *gump, CGUIExternalTexture *mapObject)
{
    Core::MappedFile& file = g_FileManager.m_MultiMap;

    if (file.GetSize() == 0u)
    {
        LOG("MultiMap.rle is not loaded!\n");
        return;
    }

    file.ResetPtr();

    int w = file.ReadLE<i32>();
    int h = file.ReadLE<i32>();

    if (w < 1 || h < 1)
    {
        LOG("Failed to load bounds from MultiMap.rle\n");
        return;
    }

    int mapSize = gump->Width * gump->Height;
    std::vector<u8> byteMap(mapSize, 0);

    int startX = gump->StartX / 2;
    int endX = gump->EndX / 2;

    int widthDivizor = endX - startX;

    if (widthDivizor == 0)
    {
        widthDivizor++;
    }

    int startY = gump->StartY / 2;
    int endY = gump->EndY / 2;

    int heightDivizor = endY - startY;

    if (heightDivizor == 0)
    {
        heightDivizor++;
    }

    int width = (gump->Width << 8) / widthDivizor;
    int height = (gump->Height << 8) / heightDivizor;

    int x = 0;
    int y = 0;

    int maxPixelValue = 1;

    while (!file.IsEOF())
    {
        u8 pic = file.ReadLE<u8>();
        u8 size = pic & 0x7F;

        bool colored = (bool)(pic & 0x80);

        int startHeight = startY * height;
        int currentHeight = y * height;
        int posY = gump->Width * ((currentHeight - startHeight) >> 8);

        for (int i = 0; i < size; i++)
        {
            if (colored && x >= startX && x < endX && y >= startY && y < endY)
            {
                int position = posY + ((width * (x - startX)) >> 8);

                u8 &pixel = byteMap[position];

                if (pixel < 0xFF)
                {
                    if (pixel == maxPixelValue)
                    {
                        maxPixelValue++;
                    }

                    pixel++;
                }
            }

            x++;

            if (x >= w)
            {
                x = 0;
                y++;
                currentHeight += height;
                posY = gump->Width * ((currentHeight - startHeight) >> 8);
            }

            //if (y >= Height)
            //	break;
        }

        //if (y >= Height)
        //	break;
    }

    if (maxPixelValue >= 1)
    {
        u16 *huesData =
            (u16 *)((u8 *)g_ColorManager.GetHuesRangePointer() + 30800); // color = 0x015C

        std::vector<u16> colorTable(maxPixelValue);
        int colorOffset = 31 * maxPixelValue;

        for (int i = 0; i < maxPixelValue; i++)
        {
            colorOffset -= 31;
            colorTable[i] = 0x8000 | huesData[colorOffset / maxPixelValue];
        }

        std::vector<u16> wordMap(mapSize);

        for (int i = 0; i < mapSize; i++)
        {
            u8 &pic = byteMap[i];

            wordMap[i] = (pic != 0u ? colorTable[pic - 1] : 0);
        }

        g_GL_BindTexture16(*mapObject->m_Texture, gump->Width, gump->Height, &wordMap[0]);
    }
}

bool CMultiMap::LoadFacet(CGumpMap *gump, CGUIExternalTexture *mapObject, int facet)
{
    if (facet < 0 || facet > 5)
    {
        LOG("Invalid facet index: %i\n", facet);

        return false;
    }

    Core::MappedFile& file = g_FileManager.m_FacetMul[facet];
    file.ResetPtr();

    if (file.GetSize() == 0u)
    {
        LOG("Facet %i is not loaded!\n", facet);

        return false;
    }

    int mapWidth = file.ReadLE<i16>();
    int mapHeight = file.ReadLE<i16>();

    //DebugMsg("Facet w:%i h:%i\n", mapWidth, mapHeight);

    int startX = gump->StartX;
    int endX = gump->EndX;

    int startY = gump->StartY;
    int endY = gump->EndY;

    int width = endX - startX;
    int height = endY - startY;

    std::vector<u16> map(width * height);

    for (int y = 0; y < mapHeight; y++)
    {
        int x = 0;
        int colorCount = file.ReadLE<i32>() / 3;

        for (int i = 0; i < colorCount; i++)
        {
            int size = file.ReadLE<u8>();
            u16 color = 0x8000 | file.ReadLE<i16>();

            for (int j = 0; j < size; j++)
            {
                if ((x >= startX && x < endX) && (y >= startY && y < endY))
                {
                    map[((y - startY) * width) + (x - startX)] = color;
                }

                x++;
            }
        }
    }

    g_GL_BindTexture16(*mapObject->m_Texture, width, height, &map[0]);

    //Или не надо...?
    mapObject->m_Texture->Width = gump->Width;
    mapObject->m_Texture->Height = gump->Height;

    return true;
}
