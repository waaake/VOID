// Copyright (c) 2025 waaake
// Licensed under the MIT License

#include <iostream>

/* Google */
#include <gtest/gtest.h>

/* Internal */
#include "Definition.h"
#include "Timecode.h"

VOID_NAMESPACE_OPEN

TEST(TimecodeTest, FramesToTimecode)
{
    Timecode tc1(1001, 24);
    EXPECT_EQ(tc1.String(), "00:00:41:17");

    Timecode tc2(60, 24);
    EXPECT_EQ(tc2.String(), "00:00:02:12");
}

TEST(TimecodeTest, TimecodeToFrames)
{
    Timecode tc1 = Timecode::Get("00:00:00:00", 24.0);
    EXPECT_EQ(tc1.frames, 0);
    EXPECT_EQ(tc1.framerate, 24.0);

    // PAL
    Timecode pal1 = Timecode::Get("00:01:00:00", 25.0);
    EXPECT_EQ(pal1.frames, 1500);

    // NTSC
    Timecode ntsc1 = Timecode::Get("01:00:00:00", 30.0);
    EXPECT_EQ(ntsc1.frames, 108000);

    Timecode random = Timecode::Get("02:15:30:15", 24.0);
    EXPECT_EQ(random.frames, 195135);
}

TEST(TimecodeTest, DropFrames)
{
    Timecode tc1(0, 29.97, true);
    EXPECT_EQ(tc1.String(), "00:00:00;00");

    Timecode tc2(1797, 29.97, true);
    EXPECT_EQ(tc2.String(), "00:00:59;27");

    Timecode tc3 = Timecode::Get("00:00:59;27", 29.97, true);
    EXPECT_EQ(tc3.frames, 1797);
}

VOID_NAMESPACE_CLOSE
