// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Buffer.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

MBuffer::MBuffer()
{
    m_Buffers.reserve(40);
}

void MBuffer::Reset()
{
    m_Numbers.clear();
    m_Indexer.clear();
}

void MBuffer::Clear()
{
    m_Numbers.clear();
    m_Indexer.clear();
    m_Buffers.clear();
}

FloatImage& MBuffer::Request(v_frame_t frame)
{
    int index = static_cast<int>(m_Numbers.size());
    m_Numbers.push_back(frame);

    if (index > static_cast<int>(m_Buffers.size()) - 1)
    {
        VOID_LOG_INFO("Creating index");
        std::lock_guard<std::mutex> lock(m_Mutex);

        m_Buffers.push_back(Image<float>::Create());
        m_Indexer[frame] = static_cast<int>(m_Buffers.size() - 1);
        return m_Buffers.back();
    }

    VOID_LOG_INFO("Using Existing");
    m_Indexer[frame] = index;
    return m_Buffers[m_Indexer[frame]];
}

FloatImage& MBuffer::At(v_frame_t frame)
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    // return m_Buffers.at(frame);
    return m_Buffers[m_Indexer[frame]];
}

VOID_NAMESPACE_CLOSE
