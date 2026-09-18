// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <charconv>
#include <cmath>

/* Internal */
#include "Timecode.h"

VOID_NAMESPACE_OPEN

char* Write(char* buffer, int val)
{
    buffer[0] = static_cast<char>('0' + (val / 10));
    buffer[1] = static_cast<char>('0' + (val % 10));
    return buffer + 2;
}

int Parse(const char* ptr)
{
    return (ptr[0] - '0') * 10 + (ptr[1] - '0');
}

std::string Timecode::String() const
{
    if (framerate < 1) return "00:00:00:00";
    int64_t total = frames;
    int hours = 0, minutes = 0, seconds = 0, frames = 0;

    if (dropframe && framerate == 29.97)
    {
        // For 29.97  drop frame math
        // drop 2 frames every minute except every 10th frame
        constexpr int dropframes = 2;
        constexpr int framesPerMin = 60 * 30; // 30fps
        constexpr int framesPer10Mins = (60 * 30 * 10) - (2 * 9);

        int64_t tens = total / framesPer10Mins;
        int64_t remainder = total % framesPer10Mins;

        // total += 18 * tens + 2 * ((remainder - 2) / framesPerMin);
        total += tens * 18;
        if (remainder >= dropframes)
            total += dropframes * ((remainder - dropframes) / framesPerMin);

        seconds = static_cast<int>(total / 30);
        frames = static_cast<int>(total % 30);
        minutes = (seconds / 60) % 60;
        hours = seconds / 3600;
        seconds %= 60;
    }
    else
    {
        int64_t totalsecs = total / framerate;
        frames = static_cast<int>(total % (int)framerate);

        hours = static_cast<int>(totalsecs / 3600);
        minutes = static_cast<int>((totalsecs % 3600) / 60);
        seconds = static_cast<int>(totalsecs % 60);
    }

    char buf[12] = {};
    char* ptr = buf;
    
    ptr = Write(ptr, hours);
    *ptr++ = ':';
    ptr = Write(ptr, minutes);
    *ptr++ = ':';
    ptr = Write(ptr, seconds);
    *ptr++ = dropframe ? ';' : ':';
    ptr = Write(ptr, frames);
    *ptr = '\0';

    return std::string(buf);
}

Timecode Timecode::Get(const std::string_view& tc, double framerate, bool dropframe)
{
    if (tc.length() < 11) return Timecode(0, framerate, dropframe);

    const char* data = tc.data();
    int h = Parse(data);
    int m = Parse(data + 3);
    int s = Parse(data + 6);
    int f = Parse(data + 9);

    int64_t total = 0;

    if (dropframe && framerate == 29.97)
    {
        int totalMins = (h * 60) + m;
        int frameToDrop = totalMins - (totalMins / 10);

        constexpr int framesPerHr = (60 * 30 * 60) - (2 * 54);
        constexpr int framesPerMin = 30 * 60;
        constexpr int dropFramesPerMin = 2;

        int framesToDrop = totalMins - (totalMins / 10);

        total = (static_cast<int64_t>(h) * framesPerHr) + (static_cast<int64_t>(m) * framesPerMin) + (static_cast<int64_t>(s) * 30) + f - (dropFramesPerMin * framesToDrop);
    }
    else
    {
        int64_t totalSecs = (static_cast<int64_t>(h) * 3600) + (m * 60) + s;
        total = (static_cast<int64_t>(framerate) * totalSecs) + f;
    }

    return Timecode(total, framerate, dropframe);
}

VOID_NAMESPACE_CLOSE
