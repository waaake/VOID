// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <cstddef>

/* Internal */
#include "Definition.h"
#include "PixReader.h"

VOID_NAMESPACE_OPEN

struct BufferData
{
    void* data;
    std::size_t size;

    unsigned int pixformat;
    int channels;
    int width, height;

    BufferData()
        : data(nullptr), size(0), pixformat(VOID_GL_UNSIGNED_BYTE)
        , channels(0), width(0), height(0) {}
    
    BufferData(void* data, std::size_t size, unsigned int pixformat, int channels, int width, int height)
        : data(data), size(size), pixformat(pixformat)
        , channels(channels), width(width), height(height) {}
};

VOID_NAMESPACE_CLOSE
