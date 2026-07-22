// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_DESCRIPTORS_H
#define _SEQUENCER_DESCRIPTORS_H

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

namespace Sequencer {

inline constexpr int RulerHeight = 30;
inline constexpr int TrackHeight = 60;
inline constexpr int TrackSpacing = 2;
inline constexpr int TrackHeaderWidth = 180;
inline constexpr int SceneWidth = 20000;
inline constexpr int SceneHeight = 4000;

inline constexpr double PixelsPerFrame = 2.0;

inline constexpr int RulerMajorTickInterval = 25;
inline constexpr int RulerMinorTickInterval = 5;
inline constexpr int PlayheadTriangleWidth = 12;
inline constexpr int PlayheadTriangleHeight = 8;

inline constexpr int ZBackground = 0;
inline constexpr int ZTrack = 100;
inline constexpr int ZTrackItem = 200;
inline constexpr int ZEffectItem = 250;
inline constexpr int ZTransition = 300;
inline constexpr int ZMarker = 400;
inline constexpr int ZSelectionBox = 500;
inline constexpr int ZPlayheadItem = 10000;

}; // namespace Sequencer

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_DESCRIPTORS_H
