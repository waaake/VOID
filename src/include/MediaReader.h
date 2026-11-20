// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _MEDIA_READER_BASE_H
#define _MEDIA_READER_BASE_H

/* STD */
#include <map>
#include <string>
#include <vector>

/* Internal */
#include "Definition.h"
#include "Audio.h"
#include "Colorspace.h"
#include "Image.h"
#include "FrameRange.h"
#include "Format.h"

VOID_NAMESPACE_OPEN

class VOID_API MediaReader
{
public:
    explicit MediaReader(const std::string& path) : m_Path(path) {}
    virtual ~MediaReader() = default;

    /// @brief Reads the image into the buffer.
    /// @param path Path to the image frame file or movie file to read.
    /// @param framenumber Frame number of the media.
    /// @param image Image buffer.
    /// @return true if the image was successfully read, else false.
    virtual bool Read(const std::string& path, v_frame_t framenumber, UInt8Image& image) = 0;

    /// @brief Reads the image into the buffer.
    /// @param path Path to the image frame file or movie file to read.
    /// @param framenumber Frame number of the media.
    /// @param image Image buffer.
    /// @return true if the image was successfully read, else false.
    virtual bool Read(const std::string& path, v_frame_t framenumber, FloatImage& image) = 0;

    /// @brief Reads the audio samples into the buffer.
    /// @param path Path to movie file or audio file to read.
    /// @param framenumber Frame number of the media.
    /// @param buffer Audio buffer for the frame.
    /// @return true if the buffer was filled and usable, else false.
    virtual bool Read(const std::string& path, v_frame_t framenumber, SharedFrameAudio& buffer) { return false; }

    /// @brief Retrieve the input colorspace and other color information of the media file.
    /// @return Returns the Media color info.
    virtual MediaColor* ColorInfo() { return &m_Format.color; };

    /// @brief Returns whether the File format supported by this reader is a movie or a single image file.
    /// @return false The image/frame is a physical image file.
    virtual bool IsMovieFormat() const = 0;

    /// @brief Returns whether the given media input has multi layers? Applies to EXRs
    /// @return true if the exr media has multi layers, else false.
    virtual bool MultiLayered() const { return m_Format.layers.size() > 1; }

    virtual std::string CurrentLayer() const { return m_Format.layer; }
    /// @brief Returns the names of the Layers available in the input.
    virtual std::vector<std::string> Layers() const { return m_Format.layers; }

    /// @brief Returns whether the provided media file has audio.
    /// @return true if media has audio which can be read from, else false.
    virtual bool HasAudio() const { return m_Format.HasAudio(); }

    /// @brief Returns the frame range of the media, if the media has a frame range, by default returns 0, 0
    ///         indicating that it's for an image, Movie Readers should return the valid frame range for it.
    /// @return MFrameRange instance with the start and end frame along with the framerate.
    virtual MFrameRange Framerange() const { return m_Format.framerange; }

    virtual MediaFormat* Info() { return &m_Format; }
    virtual void FetchInfo() = 0;

    /// @brief Read the metadata from the underlying image/frame.
    /// @return Returns Metadata from the media.
    virtual std::map<std::string, std::string> Metadata() const = 0;

    int Width() const { return m_Format.width; }
    int Height() const { return m_Format.height; }
    int Channels() const { return m_Format.channels; }
    float PixelAspect() const { return m_Format.pixelAspect; }
    void SetPixelAspect(float a) { m_Format.SetPixelAspect(a); }

protected:
    MediaFormat m_Format;
    std::string m_Path;
};

VOID_NAMESPACE_CLOSE

#endif // _MEDIA_READER_BASE_H
