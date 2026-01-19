// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_MEDIA_FRAME_H
#define _VOID_MEDIA_FRAME_H

/* STD */
#include <algorithm>
#include <mutex>

/* Internal */
#include "Definition.h"
#include "PixReader.h"

VOID_NAMESPACE_OPEN

class MediaStruct;

/**
 * Describes how any file could be classified as
 * We have basic media types, Image (or ImageSequence), Movie, Audio
 * And a generic non-media
 */
enum class VOID_API MediaType
{
    /* Describes Image single or Sequence */
    Image,
    /* Any moving images format which holds a collection of frame in a single container */
    Movie,
    /* Any format which can be just listened to */
    Audio,

    /* Generic Type to describe anything which isn't a media like .txt file or .cpp file */
    NonMedia
};

/**
 * Represents an entry of a file in the Media Filesystem
 * This usually represents a MediaFile
 * But in theory could represent any file format
 */
class VOID_API Frame
{
public:
    Frame();
    Frame(const std::string& path);
    Frame(const std::string& basepath,
        const std::string& name,
        const std::string& extension,
        v_frame_t frame = 0,
        unsigned int padding = 0,
        bool singlefile = false
    );

    ~Frame();

    /* Move */
    Frame(Frame&& other) noexcept;
    Frame& operator=(Frame&& other) noexcept;

    /* Copy */
    Frame(const Frame& other);
    Frame& operator=(const Frame& other);

    /**
     * Getters describing the Entry
     */
    inline const std::string& Fullpath() const { return m_Path; }
    inline const std::string& Basepath() const { return m_Basepath; }
    inline const std::string& Name() const { return m_Name; }
    inline const std::string& Extension() const { return m_Extension; }
    inline v_frame_t Framenumber() const { return m_Framenumber; }
    inline unsigned int Framepadding() const { return m_FramePadding; }

    /**
     * Returns True if the Media does not have a frame number on it to denote that this
     * file is a separate single entity
     */
    [[nodiscard]] inline bool SingleFile() const { return m_SingleFile; }
    [[nodiscard]] inline bool Templated() const { return m_Templated; }
    [[nodiscard]] inline bool Valid() const { return !m_Path.empty(); }

    /**
     * Validates and returns true if the other entry is similar to this
     * Basically checks if the basepath, name and extension are same
     * frame number is the only thing that can vary here
     */
    inline bool Similar(const Frame& other) const
    {
        return other.m_Basepath == m_Basepath && other.m_Name == m_Name && other.m_Extension == m_Extension;
    }

    /**
     * Validates the fullpath to see if this and the other entry are pointing to the exact same path
     */
    inline bool operator==(const Frame& other) const { return m_Path == other.m_Path; }

    /**
     * @brief Returns Shared Pointer to the ImageData
     * cached defines whether the frame needs to be read before returning
     * has no effect if the frame has already been read.
     * 
     * @param cached true if the frame is to be read before returning the pointer to it.
     * @param cached false if the pointer is to be returned no matter of the underlying data state.
     * @return SharedPixels std::shared_ptr to the ImageData which is registered as the Reader for the extension.
     */
    SharedPixels Image(bool cached = true);

    /**
     * Returns the underlying metadata from the image
     */
    inline const std::map<std::string, std::string> Metadata() const { return m_ImageData->Metadata(); }

    /* Frame Caches */
    void Cache();
    void ClearCache();

private: /* Members */
    std::string m_Path;
    std::string m_Basepath;
    std::string m_Name;
    std::string m_Extension;

    /**
     * Describes how many 0's are present pre-frame number
     * e.g. frame 1 can be 1 with 1 padding and 001 with 3 padding
     */
    unsigned int m_FramePadding;

    /**
     *  Not All files would have this
     */
    v_frame_t m_Framenumber;

    /**
     * Media is a single file and only has frame sequences
     */
    bool m_SingleFile;
    bool m_Templated;
    std::mutex m_Mutex;

    SharedPixels m_ImageData;

private: /* Methods */
    /**
     * Parses the filepath to derive tokens from the fullpath
     * like the name of the file, extension, frame number if present
     *
     * e.g. a file path is /path/to/file/on/disk/WithThisName.1001.exr
     * parsing this file would internally populate the struct with
     * basepath = /path/to/file/on/disk
     * name = WithThisName
     * frame = 1001
     * extension = exr
     *
     * and another file /path/to/another/file/on/disk/WithSomeName.jpg results in
     * basepath = /path/to/another/file/on/disk
     * name = WithSomeName
     * frame = 0
     * extension = jpg
     */
    void Parse(const std::string& path);

    /**
     * Checks and returns whether provided string (generally frames as string)
     * is a valid frame number or if it contains any unwanted character which may
     * not allow it to be converted to an integer frame number
     */
    bool ValidFrame(const std::string& framestring) const;
    /**
     * @brief Checks whether the provided string (generally frames section of the path)
     * is a valid templated frame, it is valid if the string is of the format ###, ####, #####
     * where number of '#'s represent the frame padding, or it could also be in a printf format
     * like, %03d, %04d and so on.
     * If any of these patterns are present in the string, the path is considered as a templated path
     * @param framestring 
     * @return true if the string contains the template patterns.
     * @return false if the string does not contain any template patterns.
     */
    bool ValidTemplate(const std::string& framestring) const;
    std::string PaddedFrame(v_frame_t frame) const;

    void Setup();
    void Setup(v_frame_t framenumber);

    /* Friends */
    friend class MediaStruct;
};

struct VOID_API MHelper
{
    static MediaType GetMediaType(const Frame& frame);

    /**
     * Checks if the Frame is a Media format
     */
    static inline bool IsImage(const Frame& frame) { return std::find(std::begin(m_ImageFormats), std::end(m_ImageFormats), frame.Extension()) != std::end(m_ImageFormats); }
    static inline bool IsMovie(const Frame& frame) { return std::find(std::begin(m_MovieFormats), std::end(m_MovieFormats), frame.Extension()) != std::end(m_MovieFormats); }

private:
    /**
     * To consider grouping files based on their types (extensions) using the below hardcoded extension mapping
     * This keeps it simpler for now, unless we decide on the plugin mech to dynamically allow distinuguishing file
     * types based on registered file formats (which will come soon)
     */
    /* Which can be classified as Images */
    inline static const std::string m_ImageFormats[8] = {
        "bmp",
        "dpx",
        "exr",
        "jpg",
        "jpeg",
        "png",
        "tiff",
        "tga"
    };

    /* Can be classified as Movies */
    inline static const std::string m_MovieFormats[4] = {
        "mov",
        "mp4",
        "mxf",
        "mkv"
    };
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_FRAME_H
