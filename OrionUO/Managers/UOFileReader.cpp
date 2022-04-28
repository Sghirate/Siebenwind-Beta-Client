#include "UOFileReader.h"
#include "ColorManager.h"
#include "Core/Log.h"
#include "../OrionUO.h"
#include "../StumpsData.h"

UOFileReader g_UOFileReader;

std::vector<u16> UOFileReader::GetGumpPixels(CIndexObject& io)
{
    size_t dataStart = io.Address;
    u32* lookupList  = (u32*)dataStart;

    int blocksize = io.Width * io.Height;

    std::vector<u16> pixels;

    if (blocksize == 0)
    {
        LOG_ERROR("UOFileReader", "GetGumpPixels: bad size:%i, %i", io.Width, io.Height);
        return pixels;
    }

    pixels.resize(blocksize);

    if (pixels.size() != blocksize)
    {
        LOG_ERROR(
            "UOFileReader",
            "Allocation pixels memory for GetGumpPixels failed (want size: %i)",
            blocksize);
        return pixels;
    }

    u16 color = io.Color;

    for (int y = 0; y < io.Height; y++)
    {
        int gSize = 0;

        if (y < io.Height - 1)
        {
            gSize = lookupList[y + 1] - lookupList[y];
        }
        else
        {
            gSize = (io.DataSize / 4) - lookupList[y];
        }

        GUMP_BLOCK* gmul = (GUMP_BLOCK*)(dataStart + lookupList[y] * 4);
        int pos          = (int)y * io.Width;

        for (int i = 0; i < gSize; i++)
        {
            u16 val = gmul[i].Value;

            if ((color != 0u) && (val != 0u))
            {
                val = g_ColorManager.GetColor16(val, color);
            }

            u16 a = (val != 0u ? 0x8000 : 0) | val;

            int count = gmul[i].Run;

            for (int j = 0; j < count; j++)
            {
                pixels[pos++] = a;
            }
        }
    }

    return pixels;
}

CGLTexture* UOFileReader::ReadGump(CIndexObject& io)
{
    CGLTexture* th = nullptr;

    std::vector<u16> pixels = GetGumpPixels(io);

    if (static_cast<unsigned int>(!pixels.empty()) != 0u)
    {
        th = new CGLTexture();
        g_GL_BindTexture16(*th, io.Width, io.Height, &pixels[0]);
    }

    return th;
}

std::vector<u16>
UOFileReader::GetArtPixels(u16 id, CIndexObject& io, bool run, short& width, short& height)
{
    u32 flag  = *(u32*)io.Address;
    u16* P    = (u16*)io.Address;
    u16 color = io.Color;

    std::vector<u16> pixels;

    if (!run) //raw tile
    {
        width  = 44;
        height = 44;
        pixels.resize(44 * 44, 0);

        for (int i = 0; i < 22; i++)
        {
            int start = (22 - ((int)i + 1));
            int pos   = (int)i * 44 + start;
            int end   = start + ((int)i + 1) * 2;

            for (int j = start; j < end; j++)
            {
                u16 val = *P++;

                if ((color != 0u) && (val != 0u))
                {
                    val = g_ColorManager.GetColor16(val, color);
                }

                if (val != 0u)
                {
                    val = 0x8000 | val;
                }

                pixels[pos++] = val;
            }
        }

        for (int i = 0; i < 22; i++)
        {
            int pos = ((int)i + 22) * 44 + (int)i;
            int end = (int)i + (22 - (int)i) * 2;

            for (int j = i; j < end; j++)
            {
                u16 val = *P++;

                if ((color != 0u) && (val != 0u))
                {
                    val = g_ColorManager.GetColor16(val, color);
                }

                if (val != 0u)
                {
                    val = 0x8000 | val;
                }

                pixels[pos++] = val;
            }
        }
    }
    else //run tile
    {
        int stumpIndex = 0;

        if (g_Orion.IsTreeTile(id, stumpIndex))
        {
            u16* ptr = nullptr;

            if (stumpIndex == g_StumpHatchedID)
            {
                width  = g_StumpHatchedWidth;
                height = g_StumpHatchedHeight;
                ptr    = (u16*)g_StumpHatched;
            }
            else
            {
                width  = g_StumpWidth;
                height = g_StumpHeight;
                ptr    = (u16*)g_Stump;
            }

            int blocksize = width * height;

            pixels.resize(blocksize);

            if (pixels.size() != blocksize)
            {
                LOG_ERROR(
                    "UOFileReader",
                    "Allocation pixels memory for ReadArt::LandTile failed (want size: %i)",
                    blocksize);
                pixels.clear();
                return pixels;
            }

            for (int i = 0; i < blocksize; i++)
            {
                pixels[i] = ptr[i];
            }
        }
        else
        {
            u16* ptr = (u16*)(io.Address + 4);

            width = *ptr;
            if ((width == 0) || width >= 1024)
            {
                LOG_ERROR("UOFileReader", "ReadArt: bad width:%i", width);
                return pixels;
            }

            ptr++;

            height = *ptr;

            if ((height == 0) || (height * 2) > 5120)
            {
                LOG_ERROR("UOFileReader", "ReadArt: bad height:%i", height);
                return pixels;
            }

            ptr++;

            u16* lineOffsets = ptr;
            u8* dataStart    = (u8*)ptr + (height * 2);

            int X     = 0;
            int Y     = 0;
            u16 XOffs = 0;
            u16 Run   = 0;

            int blocksize = width * height;

            pixels.resize(blocksize, 0);

            if (pixels.size() != blocksize)
            {
                LOG_ERROR(
                    "UOFileReader","Allocation pixels memory for ReadArt::StaticTile failed (want size: %i)",
                    blocksize);
                pixels.clear();
                return pixels;
            }

            ptr = (u16*)(dataStart + (lineOffsets[0] * 2));

            while (Y < height)
            {
                XOffs = *ptr;
                ptr++;
                Run = *ptr;
                ptr++;
                if (XOffs + Run >= 2048)
                {
                LOG_ERROR(
                    "UOFileReader","UOFileReader::ReadArt bad offset:%i, %i", XOffs, Run);
                    pixels.clear();
                    return pixels;
                }
                if (XOffs + Run != 0)
                {
                    X += XOffs;
                    int pos = Y * width + X;

                    for (int j = 0; j < Run; j++)
                    {
                        u16 val = *ptr++;

                        if (val != 0u)
                        {
                            if (color != 0u)
                            {
                                val = g_ColorManager.GetColor16(val, color);
                            }

                            val = 0x8000 | val;
                        }

                        pixels[pos++] = val;
                    }

                    X += Run;
                }
                else
                {
                    X = 0;
                    Y++;
                    ptr = (u16*)(dataStart + (lineOffsets[Y] * 2));
                }
            }
            if (g_Orion.IsCaveTile(id))
            {
                for (int y = 0; y < height; y++)
                {
                    int startY = (y != 0 ? -1 : 0);
                    int endY   = (y + 1 < height ? 2 : 1);

                    for (int x = 0; x < width; x++)
                    {
                        u16& pixel = pixels[y * width + x];

                        if (pixel != 0u)
                        {
                            int startX = (x != 0 ? -1 : 0);
                            int endX   = (x + 1 < width ? 2 : 1);

                            for (int i = startY; i < endY; i++)
                            {
                                int currentY = (int)y + (int)i;

                                for (int j = startX; j < endX; j++)
                                {
                                    int currentX = (int)x + (int)j;

                                    u16& currentPixel = pixels[currentY * width + currentX];

                                    if (currentPixel == 0u)
                                    {
                                        pixel = 0x8000;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return pixels;
}

CGLTexture* UOFileReader::ReadArt(u16 id, CIndexObject& io, bool run)
{
    CGLTexture* texture = nullptr;
    short width         = 0;
    short height        = 0;

    std::vector<u16> pixels = GetArtPixels(id, io, run, width, height);

    if (static_cast<unsigned int>(!pixels.empty()) != 0u)
    {
        int minX = width;
        int minY = height;
        int maxX = 0;
        int maxY = 0;

        if (!run)
        {
            maxX          = 44;
            maxY          = 44;
            bool allBlack = true;
            int pos       = 0;

            for (int i = 0; i < 44; i++)
            {
                for (int j = 0; j < 44; j++)
                {
                    if (pixels[pos++] != 0u)
                    {
                        i        = 44;
                        allBlack = false;
                        break;
                    }
                }
            }

            ((CIndexObjectLand*)&io)->AllBlack = allBlack;

            if (allBlack)
            {
                for (int i = 0; i < 22; i++)
                {
                    int start = (22 - ((int)i + 1));
                    int pos   = (int)i * 44 + start;
                    int end   = start + ((int)i + 1) * 2;

                    for (int j = start; j < end; j++)
                    {
                        pixels[pos++] = 0x8000;
                    }
                }

                for (int i = 0; i < 22; i++)
                {
                    int pos = ((int)i + 22) * 44 + (int)i;
                    int end = (int)i + (22 - (int)i) * 2;

                    for (int j = i; j < end; j++)
                    {
                        pixels[pos++] = 0x8000;
                    }
                }
            }
        }
        else
        {
            int pos = 0;

            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    if (pixels[pos++] != 0u)
                    {
                        minX = Core::Min(minX, int(x));
                        maxX = Core::Max(maxX, int(x));
                        minY = Core::Min(minY, int(y));
                        maxY = Core::Max(maxY, int(y));
                    }
                }
            }
        }

        texture = new CGLTexture();
        g_GL_BindTexture16(*texture, width, height, &pixels[0]);

        texture->ImageOffsetX = minX;
        texture->ImageOffsetY = minY;

        texture->ImageWidth  = maxX - minX;
        texture->ImageHeight = maxY - minY;
    }

    return texture;
}

CGLTexture* UOFileReader::ReadTexture(CIndexObject& io)
{
    CGLTexture* th = new CGLTexture();
    th->Texture    = 0;
    u16 color      = io.Color;

    u16 w = 64;
    u16 h = 64;

    if (io.DataSize == 0x2000)
    {
        w = 64;
        h = 64;
    }
    else if (io.DataSize == 0x8000)
    {
        w = 128;
        h = 128;
    }
    else
    {
                LOG_ERROR(
                    "UOFileReader", "ReadTexture: bad data size: %d", io.DataSize);
        delete th;
        return nullptr;
    }

    std::vector<u16> pixels(w * h);

    u16* P = (u16*)io.Address;

    for (int i = 0; i < h; i++)
    {
        int pos = (int)i * w;

        for (int j = 0; j < w; j++)
        {
            u16 val = *P++;

            if (color != 0u)
            {
                val = g_ColorManager.GetColor16(val, color);
            }

            pixels[pos + j] = 0x8000 | val;
        }
    }

    g_GL.IgnoreHitMap = true;
    g_GL_BindTexture16(*th, w, h, &pixels[0]);
    g_GL.IgnoreHitMap = false;

    return th;
}

CGLTexture* UOFileReader::ReadLight(CIndexObject& io)
{
    CGLTexture* th = new CGLTexture();
    th->Texture    = 0;

    std::vector<u16> pixels(io.Width * io.Height);

    u8* p = (u8*)io.Address;

    for (int i = 0; i < io.Height; i++)
    {
        int pos = (int)i * io.Width;

        for (int j = 0; j < io.Width; j++)
        {
            u16 val = (*p << 10) | (*p << 5) | *p;
            p++;
            pixels[pos + j] = (val != 0u ? 0x8000 : 0) | val;
        }
    }

    g_GL.IgnoreHitMap = true;
    g_GL_BindTexture16(*th, io.Width, io.Height, &pixels[0]);
    g_GL.IgnoreHitMap = false;

    return th;
}

std::pair<CGLTexture*, Core::Vec2<i16>>
UOFileReader::ReadCursor(u16 a_id, CIndexObject& a_io, bool a_run)
{
    CGLTexture* texture = nullptr;
    short width         = 0;
    short height        = 0;
    Core::Vec2<i16> hotspot;
    std::vector<u16> pixels = GetArtPixels(a_id, a_io, a_run, width, height);
    if (!pixels.empty())
    {
        for (int i = 0; i < width; i++)
        {
            u8 g = (u8)((pixels[i] >> 8) & 0xFF);
            if (g > 0)
                hotspot.x = i;

            pixels[i]                        = 0;
            pixels[(height - 1) * width + i] = 0;
        }

        for (int i = 0; i < height; i++)
        {
            u8 g = (u8)((pixels[i * width] >> 8) & 0xFF);
            if (g > 0)
                hotspot.y = i;

            pixels[i * width]             = 0;
            pixels[i * width + width - 1] = 0;
        }

        int minX = width;
        int minY = height;
        int maxX = 0;
        int maxY = 0;
        int pos  = 0;
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                if (pixels[pos++] != 0u)
                {
                    minX = Core::Min(minX, int(x));
                    maxX = Core::Max(maxX, int(x));
                    minY = Core::Min(minY, int(y));
                    maxY = Core::Max(maxY, int(y));
                }
            }
        }

        texture = new CGLTexture();
        g_GL_BindTexture16(*texture, width, height, &pixels[0]);

        texture->ImageOffsetX = minX;
        texture->ImageOffsetY = minY;

        texture->ImageWidth  = maxX - minX;
        texture->ImageHeight = maxY - minY;
    }
    return { texture, hotspot };
}