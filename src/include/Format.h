// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _MEDIA_FORMAT_H
#define _MEDIA_FORMAT_H

/* STD */
#include <string>
#include <vector>

/* Internal */
#include "Definition.h"
#include "Colorspace.h"
#include "FrameRange.h"

VOID_NAMESPACE_OPEN

/// @brief Represents all the details and aspects of a given media file
/// Expected to be a part of the MediaReader as a container which can hold internal information
/// to be shared where needed in the system
struct MediaFormat
{
    MediaColor color;
    MFrameRange framerange;
    std::string starttc;

    /// @brief Layers within the image/media file
    std::string layer;
    std::vector<std::string> layers;

    /// @brief Audio source info, even though internally the system has a fixed audio out samplerate
    /// Storing the source audio information
    int samplerate  = 0;
    int audChannels = 0;

    /// @brief dimensions
    int width       = 0;
    int height      = 0;

    int channels    = 0;
    int pixelAspect = 1;

    bool HasAudio() const { return samplerate > 0; }
    void SetResolution(int w, int h) { width = w; height = h; }
    void SetChannels(int c) { channels = c; }
};

VOID_NAMESPACE_CLOSE

#endif // _MEDIA_FORMAT_H
