// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_OIIO_READER_H
#define _VOID_OIIO_READER_H

/* STD */
#include <vector>

/* Internal */
#include "Definition.h"
#include "PixReader.h"

VOID_NAMESPACE_OPEN

class VOID_API OIIOPixReader : public VoidPixReader
{
public:
    OIIOPixReader();
    OIIOPixReader(const std::string& path);

    virtual ~OIIOPixReader();

    /**
     * Reads the provided image file's data into underlying structs
     */
    virtual void Read(const std::string& path, int framenumer) override;

    /**
     * Returns the OpenGL data type
     * e.g. GL_UNSIGNED_BYTE, GL_FLOAT
     */
    inline virtual unsigned int GLType() const override { return VOID_GL_UNSIGNED_BYTE; }

    /**
     * Specifies the number of color components in the texture
     * e.g. GL_RGBA32F | GL_RGBA32I | GL_RGBA32UI | GL_RGBA16 | GL_RGBA16F | GL_RGBA16I
     */
    inline virtual unsigned int GLInternalFormat() const override { return GLFormat(); }

    /**
     * Returns OpenGL format of the pixel data
     * GL_RGBA | GL_RGB
     */
    inline virtual unsigned int GLFormat() const override { return (m_Channels == 3) ? VOID_GL_RGB : VOID_GL_RGBA; }

    /**
     * Returns the Pointer to the underlying pixel data which will be rendered on the Renderer
     * This allows the deriving class full control over the data type, as long as the data
     * is correct to be rendered on GL Viewer, this can be returned from here
     */
    inline virtual const void* Pixels() const override { return m_Pixels.data(); }

    /**
     * Returns the frame data as unsigned char*
     * This would be used to create thumbnails for qt
     * Not all frames will be used so this function can create a vector on the fly if unsigned char
     * is not the base datatype of the class
     */
    inline virtual const unsigned char* ThumbnailPixels() override { return m_Pixels.data(); }

    /**
     * Image Specifications
     * Dimensions and Channel information for the Image
     */
    inline virtual int Width() const override { return m_Width; }
    inline virtual int Height() const override { return m_Height; }
    inline virtual int Channels() const override { return m_Channels; }

    /**
     * Clear internal pixel data
     * This is here to allow memory to be freed when needed
     */
    virtual void Clear() override;

    /**
     * Returns if the underlying struct has any pixel data
     */
    inline virtual bool Empty() const override { return m_Pixels.empty(); }

    /**
     * Retrieve the input colorspace of the media file
     */
    inline virtual ColorSpace InputColorSpace() const override { return m_InputColorSpace; }

private: /* Methods */
    std::string m_Path;

    /* Image specifications */
    int m_Width, m_Height;
    /* Number of channels in the image */
    int m_Channels;

    /* Colorspace of the Media */
    ColorSpace m_InputColorSpace;

    /* Internal data store */
    std::vector<unsigned char> m_Pixels;

};

VOID_NAMESPACE_CLOSE

#endif // _VOID_OIIO_READER_H
