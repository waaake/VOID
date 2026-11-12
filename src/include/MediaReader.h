// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _MEDIA_READER_BASE_H
#define _MEDIA_READER_BASE_H

/* STD */
#include <map>
#include <string>

/* Internal */
#include "Definition.h"
#include "Audio.h"
#include "Colorspace.h"
#include "Image.h"
#include "FrameRange.h"

VOID_NAMESPACE_OPEN

class VOID_API MediaReader
{
public:
    explicit MediaReader(const std::string& path) : m_Path(path), m_Width(0), m_Height(0), m_Channels(0) {}
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

    // /// @brief Retrieve the input colorspace and other color information of the media file.
    // /// @param path Path to the image or movie file.
    // /// @return Returns the Media color info.
    // virtual MediaColor ColorInfo(const std::string& path) const = 0;

    /// @brief Retrieve the input colorspace and other color information of the media file.
    /// @return Returns the Media color info.
    virtual MediaColor* ColorInfo() = 0;

    /// @brief Returns whether the File format supported by this reader is a movie or a single image file.
    /// @return false The image/frame is a physical image file.
    virtual bool IsMovieFormat() const = 0;

    // /// @brief Returns a samplerate if the media path has audio samples.
    // /// @return 0 if no audio can be read from the media, else the media samplerate
    // virtual int Samplerate() const { return 0; }

    /// @brief Returns whether the provided media file has audio.
    /// @return true if media has audio which can be read from, else false.
    virtual bool HasAudio() const { return false; }

    /// @brief Returns the frame range of the media, if the media has a frame range, by default returns 0, 0
    ///         indicating that it's for an image, Movie Readers should return the valid frame range for it.
    /// @return MFrameRange instance with the start and end frame along with the framerate.
    virtual MFrameRange Framerange() const { return MFrameRange(0, 0); }

    /// @brief Read the metadata from the underlying image/frame.
    /// @return Returns Metadata from the media.
    virtual std::map<std::string, std::string> Metadata() const = 0;

    int Width() const { return m_Width; }
    int Height() const { return m_Height; }
    int Channels() const { return m_Channels; }

protected:
    std::string m_Path;
    int m_Width, m_Height;
    int m_Channels;
};

VOID_NAMESPACE_CLOSE

#endif // _MEDIA_READER_BASE_H
