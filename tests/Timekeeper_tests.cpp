// Copyright (c) 2025 waaake
// Licensed under the MIT License

#include <iostream>

/* Google */
#include <gtest/gtest.h>

/* Internal */
#include "Definition.h"
#include "VoidCore/Timekeeper.h"

VOID_NAMESPACE_OPEN

TEST(TimekeeperTest, Frames)
{
    Timekeeper& t = Timekeeper::Instance();
    t.SetStart(1001);
    t.SetEnd(1100);

    EXPECT_EQ(t.StartFrame(), 1001);
    EXPECT_EQ(t.EndFrame(), 1100);

    EXPECT_EQ(t.Framerate(), 24.0);

    t.SetFramerate(60.0);
    EXPECT_EQ(t.Framerate(), 60.0);

    t.SetMediaFramerate(30.0);
    EXPECT_EQ(t.MediaFramerate(), 30.0);

    t.SetFrame(1010);
    EXPECT_EQ(t.CurrentFrame(), 1010);
    EXPECT_EQ(t.NextFrame(), 1011);
    EXPECT_EQ(t.PreviousFrame(), 1010);
}

TEST(TimekeeperTest, Display)
{
    Timekeeper& t = Timekeeper::Instance();
    t.SetStart(1001);
    t.SetEnd(1100);

    t.SetFrameDisplayMode(FrameDisplayMode::FRAMES);

    EXPECT_EQ(t.DisplayStart(), "1001");
    EXPECT_EQ(t.DisplayEnd(), "1100");

    t.SetFrameDisplayMode(FrameDisplayMode::TIMECODE);
    EXPECT_EQ(t.DisplayStart(), "00:00:41:17");
    EXPECT_EQ(t.DisplayEnd(), "00:00:45:20");
}

VOID_NAMESPACE_CLOSE
