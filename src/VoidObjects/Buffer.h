// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _MEDIA_BUFFER_H
#define _MEDIA_BUFFER_H

/* STD */
#include <deque>
#include <mutex>
#include <unordered_map>

/* Internal */
#include "Definition.h"
#include "Image.h"

VOID_NAMESPACE_OPEN

class MBuffer
{
public:
    MBuffer();

    void Reset();
    void Clear();
    bool Buffered(v_frame_t frame) const { return m_Indexer.find(frame) != m_Indexer.end(); }
    FloatImage& Request(v_frame_t frame);
    FloatImage& At(v_frame_t frame);

private:
    std::deque<v_frame_t> m_Numbers;
    std::unordered_map<v_frame_t, int> m_Indexer;
    std::vector<FloatImage> m_Buffers;
    std::mutex m_Mutex;
};

VOID_NAMESPACE_CLOSE

#endif // _MEDIA_BUFFER_H
