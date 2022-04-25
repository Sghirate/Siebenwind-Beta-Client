#include "Misc.h"
#include <locale>
#include <codecvt>

SDL_threadID g_MainThread;

int CalculatePercents(int max, int current, int maxValue)
{
    if (max > 0)
    {
        max = (current * 100) / max;

        if (max > 100)
        {
            max = 100;
        }

        if (max > 1)
        {
            max = (maxValue * max) / 100;
        }
    }

    return max;
}

#if DEBUGGING_OUTPUT == 1
void DebugMsg(const char *format, ...)
{
    va_list arg;
    va_start(arg, format);

    char buf[512] = { 0 };
    vsprintf_s(buf, format, arg);

#if defined(ORION_WINDOWS)
    OutputDebugStringA(buf);
#else
    fprintf(stdout, "%s", buf);
#endif

    va_end(arg);
}

void DebugMsg(const wchar_t *format, ...)
{
    va_list arg;
    va_start(arg, format);

    wchar_t buf[512] = { 0 };
    vswprintf_s(buf, format, arg);

    fprintf(stdout, "%ws", buf);

    va_end(arg);
}

void DebugDump(u8 *data, int size)
{
    int num_lines = size / 16;

    if (size % 16 != 0)
    {
        num_lines++;
    }

    for (int line = 0; line < num_lines; line++)
    {
        int row = 0;

        DebugMsg("%04x: ", line * 16);

        for (row = 0; row < 16; row++)
        {
            if (line * 16 + row < size)
            {
                DebugMsg("%02x ", data[line * 16 + row]);
            }
            else
            {
                DebugMsg("-- ");
            }
        }

        DebugMsg(": ");

        char buf[17] = { 0 };

        for (row = 0; row < 16; row++)
        {
            if (line * 16 + row < size)
            {
                buf[row] = (isprint(data[line * 16 + row]) != 0 ? data[line * 16 + row] : '.');
            }
        }

        DebugMsg(buf);
        DebugMsg("\n");
    }
}
#endif
