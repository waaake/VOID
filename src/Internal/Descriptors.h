// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _INTERNAL_DESCRIPTORS_H
#define _INTERNAL_DESCRIPTORS_H

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

namespace MimeTypes {

inline constexpr const char* MediaItem = "application-void/x-MediaItem";
inline constexpr const char* PlaylistItem = "application-void/x-PlaylistItem";
inline constexpr const char* TabIndex = "application-void/x-tab-index";

} // namespace MimeTypes

namespace Sequence {

enum class TrackType
{
    VIDEO,
    AUDIO
};

enum class Type
{
    VIDEO,
    AUDIO
};

} // namespace Sequence

namespace Audio {

inline constexpr int64_t OutSamplerate = 48000;
inline constexpr int64_t OutChannels = 2;

} // namespace Audio

VOID_NAMESPACE_CLOSE

#endif // _INTERNAL_DESCRIPTORS_H
