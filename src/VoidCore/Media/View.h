// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_MEDIA_VIEW_H
#define _VOID_MEDIA_VIEW_H

/* STD */
#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

/* Internal */
#include "Definition.h"
#include "Frame.h"
#include "VoidCore/MediaFilesystem.h"

VOID_NAMESPACE_OPEN

struct View
{
    MediaStruct media;
    std::unordered_map<v_frame_t, Frame> frames;
    std::vector<v_frame_t> framenumbers;

    inline bool Empty() const { return frames.empty(); }
    inline bool Valid() const { return !frames.empty(); }
    inline void Clear()
    {
        frames.clear();
        framenumbers.clear();
    }
    inline bool Contains(v_frame_t frame) const
    {
        return std::find(framenumbers.begin(), framenumbers.end(), frame) != framenumbers.end();
    }

    inline Frame GetFrame(v_frame_t frame) const { return frames.at(frame); }
};

typedef std::vector<View> Views;

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_VIEW_H
