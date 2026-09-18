// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Google */
#include <gtest/gtest.h>

/* Internal */
#include "Definition.h"
#include "FrameRange.h"

VOID_NAMESPACE_OPEN

TEST(FrameRangeTest, Construction)
{
    // Test construction and default params
    MFrameRange r(1001, 1100);

    EXPECT_EQ(r.startframe, 1001);
    EXPECT_EQ(r.endframe, 1100);
    EXPECT_EQ(r.duration, 100);
    EXPECT_EQ(r.framerate, 24.0);
}

TEST(FrameRangeTest, Ranges)
{
    // Test that the FromNumbers construction normalizes the range always
    MFrameRange r1 = MFrameRange::FromNumbers(300, 400);
    MFrameRange r2 = MFrameRange::FromNumbers(400, 300);

    EXPECT_TRUE(r1 == r2);

    MFrameRange r3(1001, 1020);
    MFrameRange r4(1010, 1030);
    MFrameRange r5(1010, 1015);

    // Test that the ranges overlap
    EXPECT_TRUE(r3.Overlaps(r4));

    // Test that the range is completely overlapped i.e. covered
    EXPECT_TRUE(r3.Covers(r5));
}

TEST(FrameRangeTest, Diff)
{
    MFrameRange r1(100, 150);
    MFrameRange r2(120, 240);

    EXPECT_EQ(r1.HeadDiff(r2), MFrameRange(100, 120));
    EXPECT_EQ(r1.TailDiff(r2), MFrameRange(150, 240));
}

TEST(FrameRangeTest, Timecode)
{
    MFrameRange r(4, 54);

    EXPECT_EQ(r.StartTC(), "00:00:00:04");
    EXPECT_EQ(r.EndTC(), "00:00:02:06");

    EXPECT_EQ(r.TC(48), "00:00:02:00");
}

VOID_NAMESPACE_CLOSE
