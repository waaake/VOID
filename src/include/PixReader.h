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
typedef std::shared_ptr<VoidPixReader> SharedPixReader;
typedef std::shared_ptr<VoidMPixReader> SharedMPixReader;

class VoidPixReader
{
public:
    VoidPixReader(const std::string& path, v_frame_t framenumber = 0)
        : m_Path(path), m_Framenumber(framenumber), m_Width(100), m_Height(100), m_Channels(0) {}
    virtual ~VoidPixReader() = default;

    /**
     * Returns the OpenGL texture type
     * e.g. GL_UNSIGNED_BYTE, GL_FLOAT
     */
    virtual unsigned int GLType() const { return VOID_GL_FLOAT; }

    /**
     * Specifies the number of color components in the texture
     * e.g. GL_RGBA32F | GL_RGBA32I | GL_RGBA32UI | GL_RGBA16 | GL_RGBA16F | GL_RGBA16I
     */
    virtual unsigned int GLInternalFormat() const { return (m_Channels == 3) ? VOID_GL_RGB : VOID_GL_RGBA; }

    /**
     * Specifies the format of the pixel data
     * GL_RGBA | GL_RGB
     */
    virtual unsigned int GLFormat() const { return (m_Channels == 3) ? VOID_GL_RGB : VOID_GL_RGBA; }

    /**
     * @brief Called for generating Thumbnail for the media and gets used to create the QImage which needs unsigned char
     * buffer to fill the QImage
     * 
     * @param path Path of the image to read.
     * @param frame Frame number.
     * @param image Unsigned char based buffer to fill.
     */
    virtual void ReadThumbnail(const std::string& path, v_frame_t frame, UInt8Image& image) = 0;

    /**
     * Image Specifications
     * Dimensions and Channel information for the Image
     */
    virtual int Width() const { return m_Width; }
    virtual int Height() const { return m_Height; }
    virtual int Channels() const { return m_Channels; }

    /**
     * Clear internal pixel data
     * This is here to allow memory to be freed when needed
     */
    virtual void Clear() = 0;

    /**
     * @brief Reads the image data and fills up the Float image buffer.
     * 
     * @param path Path to the image/movie.
     * @param frame Frame number to read.
     * @param image Float buffer to write to.
     */
    virtual void Read(const std::string& path, v_frame_t frame, FloatImage& image) = 0;

    /**
     * Retrieve the input colorspace of the media file
     * At the moment the reader is expected to output Linear colorspace,
     * but in case the reader wants a different one, can possibly override this
     */
    virtual ColorSpace InputColorSpace() const { return ColorSpace::Linear; }

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
    int m_Width, m_Height;
    int m_Channels;
};

class VoidMPixReader : public VoidPixReader
{
public:
    VoidMPixReader(const std::string& path, v_frame_t framenumber = 0) : VoidPixReader(path, framenumber) {}

    /**
     * Returns the frame range information of the Movie media
     */
    virtual MFrameRange Framerange() = 0;
    virtual bool IsMovie() const { return true; }
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PIX_READER_H
