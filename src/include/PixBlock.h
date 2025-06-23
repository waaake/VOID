#ifndef _VOID_PIXEL_BLOCK_H
#define _VOID_PIXEL_BLOCK_H

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

/**
 * These are effectively the same a GL types just named internally as per "VOID Conventions"
 */
#define VOID_GL_BYTE 0x1400
#define VOID_GL_UNSIGNED_BYTE 0x1401
#define VOID_GL_SHORT 0x1402
#define VOID_GL_UNSIGNED_SHORT 0x1403
#define VOID_GL_INT 0x1404
#define VOID_GL_UNSIGNED_INT 0x1405
#define VOID_GL_FLOAT 0x1406
#define VOID_GL_2_BYTES 0x1407
#define VOID_GL_3_BYTES 0x1408
#define VOID_GL_4_BYTES 0x1409
#define VOID_GL_DOUBLE 0x140A

/**
 * GL Formats but with "VOID Conventions"
 */
#define VOID_GL_RED 0x1903
#define VOID_GL_GREEN 0x1904
#define VOID_GL_BLUE 0x1905
#define VOID_GL_ALPHA 0x1906
#define VOID_GL_RGB 0x1907
#define VOID_GL_RGBA 0x1908
#define VOID_GL_LUMINANCE 0x1909
#define VOID_GL_LUMINANCE_ALPHA 0x190A


struct PixBlock
{
    virtual ~PixBlock() {}

    /**
     * Returns the OpenGL texture type
     * e.g. GL_UNSIGNED_BYTE, GL_FLOAT
     */
    virtual unsigned int GLType() const = 0;

    /**
     * Returns OpenGL channel format
     * GL_RGBA | GL_RGB
     */
    virtual unsigned int GLFormat() const = 0;

    /**
     * Returns the Pointer to the underlying pixel data which will be rendered on the Renderer
     * This allows the deriving class full control over the data type, as long as the data
     * is correct to be rendered on GL Viewer, this can be returned from here
     */
    virtual const void* Pixels() const = 0;

    /**
     * Returns the frame data as unsigned char*
     * This would be used to create thumbnails for qt
     * Not all frames will be used so this function can create a vector on the fly if unsigned char
     * is not the base datatype of the class
     */
    virtual const unsigned char* ThumbnailPixels() const = 0;

    /**
     * Image Specifications
     * Dimensions and Channel information for the Image
     */
    virtual int Width() const = 0;
    virtual int Height() const = 0;
    virtual int Channels() const = 0;

    /**
     * Clear internal pixel data
     * This is here to allow memory to be freed when needed
     */
    virtual void Clear() = 0;

    /**
     * Returns if the underlying struct has any pixel data
     */
    virtual bool Empty() = 0;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PIXEL_BLOCK_H
