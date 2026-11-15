// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _IMAGE_H
#define _IMAGE_H

/* STD */
#include <cstring>
#include <memory>
#include <vector>

/* Internal */
#include "Definition.h"
// #include "Allocator.h"
#include "Colorspace.h"
#include "Row.h"

VOID_NAMESPACE_OPEN

// These are effectively the same a GL types just named internally as per "VOID Conventions"
#define VOID_GL_BYTE            0x1400
#define VOID_GL_UNSIGNED_BYTE   0x1401
#define VOID_GL_SHORT           0x1402
#define VOID_GL_UNSIGNED_SHORT  0x1403
#define VOID_GL_INT             0x1404
#define VOID_GL_UNSIGNED_INT    0x1405
#define VOID_GL_FLOAT           0x1406
#define VOID_GL_2_BYTES         0x1407
#define VOID_GL_3_BYTES         0x1408
#define VOID_GL_4_BYTES         0x1409
#define VOID_GL_DOUBLE          0x140A

// GL Formats but with "VOID Conventions"
#define VOID_GL_RED             0x1903
#define VOID_GL_GREEN           0x1904
#define VOID_GL_BLUE            0x1905
#define VOID_GL_ALPHA           0x1906
#define VOID_GL_RGB             0x1907
#define VOID_GL_RGBA            0x1908
#define VOID_GL_LUMINANCE       0x1909
#define VOID_GL_LUMINANCE_ALPHA 0x190A
#define VOID_GL_RGBA32F         0x8814
#define VOID_GL_RGB32F          0x8815
#define VOID_GL_RGBA16F         0x881A
#define VOID_GL_RGB16F          0x881B
#define VOID_GL_RGBA32UI        0x8D70
#define VOID_GL_RGB32UI         0x8D71
#define VOID_GL_RGBA16UI        0x8D76
#define VOID_GL_RGB16UI         0x8D77
#define VOID_GL_RGBA8UI         0x8D7C
#define VOID_GL_RGB8UI          0x8D7D
#define VOID_GL_RGBA32I         0x8D82
#define VOID_GL_RGB32I          0x8D83
#define VOID_GL_RGBA16I         0x8D88
#define VOID_GL_RGB16I          0x8D89
#define VOID_GL_RGBA8I          0x8D8E
#define VOID_GL_RGB8I           0x8D8F

// Textures Output format
#define VOID_GL_COMPRESSED_RED       0x8225
#define VOID_GL_COMPRESSED_RG        0x8226
#define VOID_GL_RG                   0x8227
#define VOID_GL_RG_INTEGER           0x8228
#define VOID_GL_R8                   0x8229
#define VOID_GL_R16                  0x822A
#define VOID_GL_RG8                  0x822B
#define VOID_GL_RG16                 0x822C
#define VOID_GL_R16F                 0x822D
#define VOID_GL_R32F                 0x822E
#define VOID_GL_RG16F                0x822F
#define VOID_GL_RG32F                0x8230
#define VOID_GL_R8I                  0x8231
#define VOID_GL_R8UI                 0x8232
#define VOID_GL_R16I                 0x8233
#define VOID_GL_R16UI                0x8234
#define VOID_GL_R32I                 0x8235
#define VOID_GL_R32UI                0x8236
#define VOID_GL_RG8I                 0x8237
#define VOID_GL_RG8UI                0x8238
#define VOID_GL_RG16I                0x8239
#define VOID_GL_RG16UI               0x823A
#define VOID_GL_RG32I                0x823B
#define VOID_GL_RG32UI               0x823C

template <typename _Ty>
struct Buffer
{
    // Adds additional time for memory management when clearing underlying vector data
    // Needs rework on the approach, possibly lock-free mechanism and using std::vector over std::unordered_map
    // std::vector<_Ty, ImageBufferAllocator<_Ty>> _buf;
    std::vector<_Ty> _buf;
    const _Ty* Data() const noexcept { return _buf.data(); }
    _Ty* Data() noexcept { return _buf.data(); }
    std::size_t Size() const noexcept { return _buf.size(); }
    
    const auto& At(std::size_t _index) const noexcept { return _buf.at(_index); }
    auto& operator[](std::size_t _index) noexcept { return _buf[_index]; }
    bool Empty() const noexcept { return _buf.empty(); }
    void Resize(std::size_t size)
    {
        _buf.resize(size);
        if (_buf.capacity() > _buf.size()) _buf.shrink_to_fit();
    }
    void Clear()
    {
        if (_buf.empty()) return;

        _buf.clear();
        _buf.shrink_to_fit();
    }
};

template <typename _Ty>
struct Image
{
    int width;
    int height;
    int channels;

    // Format
    unsigned int type;
    unsigned int format;
    unsigned int pixfmt;

    Buffer<_Ty> buffer;

    // Kind of like a hash to ensure that we know that the size or the internal format has changed in the image
    unsigned int InternalFormat() const { return static_cast<unsigned int>(buffer.Size()) * format * pixfmt; }
    std::size_t Size() const noexcept { return buffer.Size() * sizeof(_Ty); }
    bool Valid() const noexcept { return !buffer.Empty(); }
    bool Empty() const noexcept { return buffer.Empty(); }
    void Clear()
    {
        buffer.Clear();
        editable.Clear();
    }
    const _Ty* Pixels() const noexcept { return editable.Empty() ? buffer.Data() : editable.Data(); }
    _Ty* Writable() noexcept { return buffer.Data(); }
    void CreateEditable()
    {
        if (editable.Size() != buffer.Size())
            editable.Resize(buffer.Size());
        std::memcpy(editable.Data(), buffer.Data(), buffer.Size() * sizeof(_Ty));
    }
    void Resize() { buffer.Resize(width * height * channels); }
    void Resize(std::size_t size) { buffer.Resize(size); }
    ImageRow Row(std::size_t row)
    {
        return (row >= height) ? ImageRow() : ImageRow(buffer.Data(), row, width, channels, sizeof(_Ty));
    }
    ImageRow EditableRow(std::size_t row)
    {
        return (row >= height) ? ImageRow() : ImageRow(editable.Data(), row, width, channels, sizeof(_Ty));
    }

    explicit operator bool() const noexcept { return !buffer.Empty(); }

    Image() {}
    static std::shared_ptr<Image<_Ty>> Create() { return std::make_shared<Image<_Ty>>(); }
private:
    Buffer<_Ty> editable;
};

typedef std::shared_ptr<Image<float>> FloatImage;
typedef std::shared_ptr<Image<unsigned char>> UInt8Image;

VOID_NAMESPACE_CLOSE

#endif // _IMAGE_H
