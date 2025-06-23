#ifndef _VOID_OIIO_READER_H
#define _VOID_OIIO_READER_H

/* STD */
#include <vector>

/* Internal */
#include "Definition.h"
#include "ImageReader.h"
#include "FormatForge.h"

#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

struct OIIOPixBlock : public PixBlock
{
    /* Constructor */
    OIIOPixBlock(const std::vector<unsigned char> pixels, const int width, const int height, const int channels);

    /* Destructor */
    virtual ~OIIOPixBlock();

    /**
     * Returns the OpenGL texture type
     * e.g. GL_UNSIGNED_BYTE, GL_FLOAT
     */
    virtual unsigned int GLType() const override { return VOID_GL_UNSIGNED_BYTE; }

    /**
     * Returns OpenGL channel format
     * GL_RGBA | GL_RGB
     */
    virtual unsigned int GLFormat() const override { return m_Format; }

    /**
     * Returns the Pointer to the underlying pixel data which will be rendered on the Renderer
     */
    virtual const void* Pixels() const override { return m_Pixels.data(); }

    /**
     * Return pixels as unsigned char* for Thumbnails
     */
    virtual const unsigned char* ThumbnailPixels() const override { return m_Pixels.data(); }

    /**
     * Image Specifications
     * Dimensions and Channel information for the Image
     */
    virtual int Width() const override { return m_Width; }
    virtual int Height() const override { return m_Height; }
    virtual int Channels() const override { return m_Channels; }

    /**
     * Clear any pixel data
     */
    virtual void Clear() override { m_Pixels.clear(); VOID_LOG_INFO("Frame Cleared.."); }

    /**
     * Returns if the underlying data structure has any pixel data
     */
    virtual bool Empty() override { return m_Pixels.empty(); }

private: /* Members */
    /* GL Image Format to be rendered */
    unsigned int m_Format;

    /* Image Specification */
    int m_Width, m_Height, m_Channels;

    /* Pixel Data */
    std::vector<unsigned char> m_Pixels;
};

class OIIOReader : public ImageReader
{
    SharedPixBlock Read(const std::string& path);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_OIIO_READER_H
