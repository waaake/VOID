// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_PIX_READER_H
#define _VOID_PIX_READER_H

/* STD */
#include <map>
#include <memory>
#include <string>

/* Internal */
#include "Colorspace.h"
#include "Definition.h"
#include "FrameRange.h"
#include "Image.h"
#include "Row.h"

VOID_NAMESPACE_OPEN

/* Typedefs */
class VoidPixReader;    // forward decl for the typedef
class VoidMPixReader;
typedef std::shared_ptr<VoidPixReader> SharedPixels;
typedef std::shared_ptr<VoidMPixReader> SharedMPixels;

class VoidPixReader
{
public:
    VoidPixReader(const std::string& path, v_frame_t framenumber = 0) : m_Path(path), m_Framenumber(framenumber) {}
    virtual ~VoidPixReader() = default;

    virtual SharedPixels Copy() const = 0;

    /**
     * Returns the OpenGL texture type
     * e.g. GL_UNSIGNED_BYTE, GL_FLOAT
     */
    virtual unsigned int GLType() const = 0;

    /**
     * Specifies the number of color components in the texture
     * e.g. GL_RGBA32F | GL_RGBA32I | GL_RGBA32UI | GL_RGBA16 | GL_RGBA16F | GL_RGBA16I
     */
    virtual unsigned int GLInternalFormat() const = 0;

    /**
     * Specifies the format of the pixel data
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
     * @brief Returns a pointer to the underlying buffer which can be written to, this will be used
     * for additional processing that will be applied on the image data
     * 
     * @return void* Writeable pointer to the buffer.
     */
    virtual void* Writable() = 0;

    /**
     * @brief Returns the width wide buffer for the requested row. This holds the information of pixels * channels
     * for the full width of the image i.e. one set of pixel rows.
     * 
     * @param row Index of the row of pixels required.
     * @return ImageRow Buffer.
     */
    virtual ImageRow Row(std::size_t row) = 0;

    /**
     * Returns the frame data as unsigned char*
     * This would be used to create thumbnails for qt
     * Not all frames will be used so this function can create a vector on the fly if unsigned char
     * is not the base datatype of the class
     */
    virtual const unsigned char* ThumbnailPixels() = 0;
    virtual void ReadThumbnail(const std::string& path, v_frame_t frame, UInt8Image& image) = 0;

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
    virtual bool Empty() const = 0;

    /**
     * Reads the image at the given path
     * updates the underlying struct with the data
     */
    virtual void Read() = 0;
    virtual void Read(const std::string& path, v_frame_t frame, FloatImage& image) = 0;

    /**
     * Returns the Size of the frame data
     */
    virtual size_t FrameSize() const = 0;

    /**
     * Retrieve the input colorspace of the media file
     */
    virtual ColorSpace InputColorSpace() const = 0;

    /**
     * @brief Returns whether the given Image/Frame is a Movie
     * 
     * @return true The image/frame belongs to a movie.
     * @return false The image/frame is a physical image file.
     */
    virtual bool IsMovie() const { return false; }

    /**
     * @brief Returns the Number of channels in the Audio Stream.
     * 
     * @return int Channel count for the Audio stream.
     */
    virtual int AudioChannels() const { return 0; };

    /**
     * @brief Returns the number of samples for the Audio.
     * 
     * @return int
     */
    virtual int Samplerate() const { return 0; }

    /**
     * @brief Returns the Audio Samples 
     * 
     * @return const unsigned char* Audio stream data.
     */
    virtual const unsigned char* AudioSamples() const { return nullptr; }

    /**
     * Read the metadata from the underlying image/frame
     * Returns with all the keys that can be read from the metadata
     * so keys might not be same/static
     */
    virtual const std::map<std::string, std::string> Metadata() const = 0;

    inline std::string Framepath() const { return m_Path; }
    inline v_frame_t Framenumber() const { return m_Framenumber; }

protected:
    std::string m_Path;
    v_frame_t m_Framenumber;
};

class VoidMPixReader : public VoidPixReader
{
public:
    VoidMPixReader(const std::string& path, v_frame_t framenumber = 0) : VoidPixReader(path, framenumber) {}

    /**
     * Returns the frame range information of the Movie media
     */
    virtual MFrameRange Framerange() = 0;

    /**
     * Returns the framerate of the movie media
     */
    virtual double Framerate() = 0;
    virtual bool IsMovie() const { return true; }
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PIX_READER_H
