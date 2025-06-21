#ifndef _VOID_TIMECODE_H
#define _VOID_TIMECODE_H

/* STD */
#include <string>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

struct Timecode
{
    int hours;
    int minutes;
    int seconds;
    int frames;
    bool dropframe;
};

class VoidTC
{
    static Timecode GetTimecode(const int frame, const int framerate);
    static Timecode GetTimecodeNeon(const int frame, const int framerate);
    static Timecode GetTimecodeSSE2(const int frame, const int framerate);

public:
    VOID_API static std::string TimecodeString(const int frame, const int framerate);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_TIMECODE_H
