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

enum class BufferType : uint8_t
{
    Uint8,
    Uint16,
    Float
};

enum class MovieCodec : uint8_t
{
    H264,
    DNXHD,
    MJPEG,
    MPEG4,
    PRORES
};

struct EncodeSpec
{
    int width, height;
    int channels;
    float rate = { 24.f };
    BufferType type = { BufferType::Uint8 };
    MovieCodec codec = { MovieCodec::H264 };
    bool respeed = { false };

    EncodeSpec(int w, int h, int ch, const BufferType& ty)
        : width(w), height(h), channels(ch), type(ty) {}

    EncodeSpec(int w, int h, int ch, float r, const BufferType& ty, const MovieCodec& codec, bool respeed)
        : width(w), height(h), channels(ch), rate(r), type(ty), codec(codec), respeed(respeed) {}
};

struct InputSpec
{
    int width, height;
    int channels;
    BufferType type = { BufferType::Uint8 };

    InputSpec(int w, int h, int ch, const BufferType& type) : width(w), height(h), channels(ch), type(type) {}
};

class PixWriter
{
public:
    PixWriter(const EncodeSpec& spec) : m_Spec(spec) {}
    virtual ~PixWriter() = default;

    virtual bool Setup(const std::string& path) = 0;
    virtual bool AddBuffer(const void* buffer, std::size_t size, const InputSpec& spec) = 0;
    virtual bool Write() = 0;
    virtual void Cleanup() = 0;

protected:
    EncodeSpec m_Spec;
};

VOID_NAMESPACE_CLOSE

#endif // _PIX_WRITER_H
