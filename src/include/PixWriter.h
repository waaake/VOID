// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _PIX_WRITER_H
#define _PIX_WRITER_H

/* STD */
#include <string>
#include <vector>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

enum class WriterType
{
    Image,
    Movie
};

enum class BufferType
{
    Uint8,
    Uint16,
    Float
};

class PixWriter
{
public:
    // PixWriter(int width, int height, int channels, const WriterType& type = WriterType::Image) : PixWriter(width, height, channels, type) {}
    PixWriter(int width, int height, int channels, const WriterType& type) : m_Width(width), m_Height(height), m_Channels(channels), m_Type(type) {}
    virtual ~PixWriter() = default;

    virtual bool Setup(const std::string& path) = 0;
    virtual bool AddBuffer(const void* buffer, std::size_t size, const BufferType& type) = 0;
    virtual bool Write() = 0;
    virtual void Cleanup() = 0;

protected:
    WriterType m_Type;
    int m_Width, m_Height;
    int m_Channels;
};

VOID_NAMESPACE_CLOSE

#endif // _PIX_WRITER_H
