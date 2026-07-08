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
inline constexpr int TrackHeaderWidth = 120;
inline constexpr int SceneWidth = 20000;
inline constexpr int SceneHeight = 4000;

inline constexpr double PixelsPerFrame = 2.0;

}; // namespace Sequencer

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_DESCRIPTORS_H
