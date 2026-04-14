// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

/**
 * @brief An ImageRow is what it looks like, a row 'of data'
 * The data in this case it represents is image pixels, an image is just a collection of
 * pixels from an actual on disk image or a frame of a movie which is being read
 * 
 * If an Image is of the size Width x Height (dimensions of the image in 2D space), a Row represents pixels along the width
 * and it can be said that the Image consists of Height number of Rows
 * 
 * For a 16 x 5 Image, we have 5 Rows of 16 Pixels each
 * 
 * Pixels will have n channels each
 * 
 * [][][][][][][][][][][][][][][][] Row 0
 * [][][][][][][][][][][][][][][][] Row 1
 * [][][][][][][][][][][][][][][][] Row 2
 * [][][][][][][][][][][][][][][][] Row 3
 * [][][][][][][][][][][][][][][][] Row 4
 * 
 */
struct ImageRow
{
    void* buffer;
    std::size_t width;
    std::size_t channels;
    std::size_t stride;

    ImageRow() : buffer(nullptr), width(0), channels(0), stride(0) {}

    /**
     * @brief Construct an ImageRow of Pixels.
     * 
     * @param buffer The completed data from the image.
     * @param row Row being requested. e.g. row 1, row 5...
     * @param width Width of the image.
     * @param channels Number of channels in the image (size of each pixel).
     * @param stride Stride represents the underlying data type size, since the buffer is a void* this represents
     *      how many bytes to offset to move to the next channel in the same pixel.
     */
    ImageRow(void* buffer, std::size_t row, std::size_t width, std::size_t channels, std::size_t stride)
        : width(width), channels(channels), stride(stride)
    {
        this->buffer = static_cast<std::byte*>(buffer) + row * width * channels * stride;
    }

    /**
     * @brief Each pixel is a collection of n channels, and the channels are defined in the image
     * returns the pointer to the start of the pixel which can be iterated upon (max: channel count).
     * 
     * @tparam _Ty Based on the image data, the type is specfied. e.g. float pixels or unsigned char pixels or uint16_t pixels
     * @param column The pixel number to read from the Row of pixels.
     * @return _Ty* Pointer to the start of the pixel data.
     */
    template<typename _Ty>
    _Ty* Pixel(std::size_t column)
    {
        return reinterpret_cast<_Ty*>(static_cast<std::byte*>(buffer) + column * channels * stride);
    }

    inline explicit operator bool() { return (bool)buffer; }
};

VOID_NAMESPACE_CLOSE
