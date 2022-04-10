#include "DateTimeStamp.h"

#if defined(ORION_WINDOWS)

std::string GetBuildDateTimeStamp()
{
    return std::string(__DATE__);
}

#else

#include "GitRevision.h"
std::string GetBuildDateTimeStamp()
{
    return std::string(__DATE__ " " __TIME__ " (" GIT_REV_STR ")");
}

#endif
