#ifndef _VOID_MEDIA_FILE_SYSTEM_H
#define _VOID_MEDIA_FILE_SYSTEM_H

/* STD */
#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>


/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

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
class VOID_API MEntry
{
public:
    MEntry();
    MEntry(const std::string& path);

    ~MEntry();

    /**
     * Getters describing the Entry
     */
    inline std::string Fullpath() const { return m_Path; }
    inline std::string Basepath() const { return m_Basepath; }
    inline std::string Name() const { return m_Name; }
    inline std::string Extension() const { return m_Extension; }
    inline v_frame_t Framenumber() const { return m_Framenumber; }
    
    /**
     * Returns True if the Media does not have a frame number on it to denote that this
     * file is a separate single entity
     */
    [[nodiscard]] inline bool SingleFile() const { return m_SingleFile; }
    [[nodiscard]] inline bool Valid() const { return !m_Path.empty(); }

    /**
     * Validates and returns true if the other entry is similar to this
     * Basically checks if the basepath, name and extension are same
     * frame number is the only thing that can vary here
     */
    inline bool Similar(const MEntry& other) const
    {
        return other.m_Basepath == m_Basepath && other.m_Name == m_Name && other.m_Extension == m_Extension;
    }

    /**
     * Validates the fullpath to see if this and the other entry are pointing to the exact same path
     */
    inline bool operator==(const MEntry& other) const { return m_Path == other.m_Path; }

private: /* Members */
    std::string m_Path;
    std::string m_Basepath;
    std::string m_Name;
    std::string m_Extension;

    /**
     *  Not All files would have this
     */
    v_frame_t m_Framenumber;

    /**
     * Media is a single file and only has frame sequences
     */
    bool m_SingleFile;

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
};

struct MHelper
{
    static MediaType GetMediaType(const MEntry& entry);

    /**
     * Checks if the MEntry is a Media format
     */
    static inline bool IsImage(const MEntry& entry) { return std::find(std::begin(m_ImageFormats), std::end(m_ImageFormats), entry.Extension()) != std::end(m_ImageFormats); }
    static inline bool IsMovie(const MEntry& entry) { return std::find(std::begin(m_MovieFormats), std::end(m_MovieFormats), entry.Extension()) != std::end(m_MovieFormats); }

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

/**
 * Collection of Media Entries (one or more)
 * This is what represents a Media Entity
 * like an Image Sequence where it holds the MEntry(ies) for the image sequence
 * or it could hold a single MEntry representing a movie file format
 */
class VOID_API MediaStruct
{
public:
    MediaStruct();
    /**
     * The MEntry is used to initialize the MediaStruct hence representing itself
     * and then it may be used to validate other entries against this
     * like comparison by the name, and extension
     */
    MediaStruct(const MEntry& entry, const MediaType& type);

    ~MediaStruct();

    /**
     * Move and Copy
     */
    MediaStruct(const MediaStruct& other);

    MediaStruct operator=(MediaStruct&& other) noexcept;
    MediaStruct operator=(const MediaStruct& other);

    /**
     * (Re)Initializes the MediaStruct
     * Usually should be called only once, as this resets the underlying paramters to now
     * consider the MediaStruct composed of files having name and extension provided entry
     */
    inline void Set(const MEntry& entry, const MediaType& type) { Reset(entry, type); }

    /**
     * Adds an entry to the struct
     */
    void Add(const MEntry& entry);

    /**
     * Validates the entry based on it's name and filetype (extension)
     * to indicate whether this entry belongs to the MediaStruct or not
     * e.g. the MediaStruct currently represents entries with name "SomeNamed"
     * and extension "jpg"
     * if another entry with name "TestNamed" is passed this method will return false
     * as the name is different and similarly if another entry with name "SomeNamed" but
     * extension "txt" is passed, this still returns false indicating the entry is odd
     * and does not match what this struct represents
     */
    bool Validate(const MEntry& entry);

    std::string Name() const;
    std::string Extension() const;
    std::string Basepath() const;

    std::string FirstPath() const;
    [[nodiscard]] bool SingleFile() const;

    /**
     * Returns whether the media struct is currently empty
     */
    [[nodiscard]] inline bool Empty() const { return m_Entries.empty(); }

    /**
     * Returns if the MediaStruct is a valid Media Type
     */
    inline bool ValidMedia() const { return (m_MediaType == MediaType::Image) || (m_MediaType == MediaType::Movie); }

    /**
     * Returns an empty entry if the struct is empty
     * else returns the first entry from the map (which isn't guaranteed to be same unless it's a single file)
     * TODO: Check on it's usecase on how this evolves, generally it should be used only for single files
     * but could have a potential usecase for multi files as well like an image sequence else the underlying
     * struct needs to be ordered map
     */
    MEntry First() const;

    inline MediaType Type() const { return m_MediaType; }

    /**
     * Constructs a MediaStuct based on a file
     * Looks up at the directory to include any files of the same naming under the media struct
     */
    static MediaStruct FromFile(const std::string& filepath);

private: /* Members */
    std::vector<v_frame_t> m_Frames;
    std::unordered_map<v_frame_t, MEntry> m_Entries;

    /* The kind of media */
    MediaType m_MediaType;

private: /* Methods */
    /**
     * Resets underlying map to now start housing the entries represented by the provided first entry
     */
    void Reset(const MEntry& entry, const MediaType& type);

    /* Clears underlying structs */
    void Clear();

private: /* Iterator */
    /**
     * Custom Iterator for the MediaStruct to allow iterating over just the entries
     * leaving the other internals
     */
    class Iterator
    {
        using Iter = std::unordered_map<v_frame_t, MEntry>::const_iterator;
        Iter it;

    public:
        explicit Iterator(Iter it)
            : it(it) {}

        /* Dereference operator */
        const MEntry& operator*() const { return it->second; }
        /* Arrow operator */
        const MEntry* operator->() const { return &(it->second); }

        /**
         * Increment operator
         * Increment internal iterator
         */
        Iterator& operator++() { ++it; return *this; }

        /* Not Equals */
        bool operator!=(const Iterator& other) const { return it != other.it; }
    };

public: /* Iterator */
    Iterator begin() const { return Iterator(m_Entries.begin()); }
    Iterator end() const { return Iterator(m_Entries.end()); }

};

class VOID_API MediaFS
{
public:
    inline MediaStruct GetMediaStruct(const std::string& filepath) { return MediaStruct::FromFile(filepath); }

    /**
     * Provides a vector of MediaStructs for a provided Directory
     */
    std::vector<MediaStruct> FromDirectory(const std::string& directory);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_FILE_SYSTEM_H
