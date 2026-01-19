// Copyright (c) 2025 waaake
// Licensed under the MIT License

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
#include "Frame.h"

VOID_NAMESPACE_OPEN

/**
 * @brief Collection of Media Entries (one or more)
 * This is what represents a Media Entity from the filesystem
 * like an Image Sequence where it holds the Frame(s) for the image sequence
 * or it could hold a single Frame representing a movie file format.
 * Each View of an Image sequence is treated as a separate Entity (or MediaStruct) because it's totally
 * different Sequence, a collection of these MediaStructs or Entities form the internal Media Entity
 */
class VOID_API MediaStruct
{
public:
    MediaStruct();
    /**
     * @brief The Frame is used to initialize the MediaStruct hence representing itself
     * and then it may be used to validate other entries against this
     * like comparison by the name, and extension
     */
    MediaStruct(Frame& frame, const MediaType& type);

    /**
     * Deserialization constructors
     */
    MediaStruct(const std::string& basepath, const std::string& name, const std::string& extension);

    MediaStruct(const std::string& basepath,
            const std::string& name,
            const std::string& extension,
            v_frame_t start,
            v_frame_t end,
            unsigned int padding
    );
    MediaStruct(const std::string& basepath,
            const std::string& name,
            const std::string& extension,
            v_frame_t start,
            v_frame_t end,
            unsigned int padding,
            const std::vector<v_frame_t>& missing
    );

    ~MediaStruct();

    /**
     * Move and Copy
     */
    MediaStruct(const MediaStruct& other);
    MediaStruct(MediaStruct&& other) noexcept;

    MediaStruct operator=(MediaStruct&& other) noexcept;
    MediaStruct operator=(const MediaStruct& other);

    /**
     * (Re)Initializes the MediaStruct
     * Usually should be called only once, as this resets the underlying paramters to now
     * consider the MediaStruct composed of files having name and extension provided entry
     */
    inline void Set(Frame& frame, const MediaType& type) { Reset(frame, type); }

    /**
     * Adds an entry to the struct
     */
    void Add(Frame& frame);

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
    bool Validate(const Frame& frame);

    std::string Name() const;
    std::string Extension() const;
    std::string Basepath() const;

    std::string FirstPath() const;
    [[nodiscard]] bool SingleFile() const;

    /*
     * Returns whether a given frame falls in the range of Media
     * i.e. between the first and the last frame of media
     * Any frame missing does not matter as this method only returns whether a frame is in the range or not
     */
    [[nodiscard]] inline bool HasFrame(v_frame_t frame) const { return frame >= m_Start && frame <= m_End; }

    /* 
     * Returns whether a given frame is available to read
     * There could be a scenario where the given frame is in the range of first - last but is not available
     * and is referred to as the missing frame.
     */
    [[nodiscard]] inline bool Contains(v_frame_t frame) const { return true; }

    /*
     * Based on the available frames, returns the frame which is just lower than the provided frame
     * This is used when the current frame is not available but we want the neartest frame to be used in it's place
     */
    v_frame_t NearestFrame(v_frame_t frame) const;

    inline v_frame_t Startframe() const { return m_Start; }
    inline v_frame_t Endframe() const { return m_End; }
    inline v_frame_t Duration() const { return (m_End - m_Start) + 1; }

    unsigned int Framepadding() const;

    /**
     * Returns whether the media struct is currently empty
     */
    [[nodiscard]] inline bool Empty() const noexcept { return m_Entries.empty(); }
    [[nodiscard]] inline size_t Size() const noexcept { return m_Entries.size(); }

    /**
     * Returns if the MediaStruct is a valid Media Type
     */
    inline bool ValidMedia() const noexcept { return (m_MediaType == MediaType::Image) || (m_MediaType == MediaType::Movie); }

    inline SharedPixels Image(v_frame_t frame, bool cached = true) { return m_Entries.at(frame).Image(cached); }
    inline const std::map<std::string, std::string> Metadata() const { return m_Entries.at(m_Start).Metadata(); }

    /**
     * @brief Process all the Frame Entries from the struct
     * if it's a valid Media, all of it's underlying frames are processed and ready to be cached/read upon demand.
     * This initialises the range of the media and missing frames information in case of an image sequence.
     * 
     */
    void Process();

    /**
     * Returns an empty entry if the struct is empty
     * else returns the first entry from the map (which isn't guaranteed to be same unless it's a single file)
     * TODO: Check on it's usecase on how this evolves, generally it should be used only for single files
     * but could have a potential usecase for multi files as well like an image sequence else the underlying
     * struct needs to be ordered map
     */
    Frame First() const;

    inline MediaType Type() const { return m_MediaType; }

    /**
     * Constructs a MediaStuct based on a file
     * Looks up at the directory to include any files of the same naming under the media struct
     */
    static MediaStruct FromFile(const std::string& filepath);

    /* Clears underlying structs */
    void Clear();
    void ClearCache();

    inline void Cache(v_frame_t frame) { m_Entries.at(frame).Cache(); }
    inline void ClearCache(v_frame_t frame) { m_Entries.at(frame).ClearCache(); }

    inline std::unordered_map<v_frame_t, Frame>::iterator begin() noexcept { return m_Entries.begin(); }
    inline std::unordered_map<v_frame_t, Frame>::iterator end() noexcept { return m_Entries.end(); }

private: /* Members */
    v_frame_t m_Start, m_End;
    /* The kind of media */
    MediaType m_MediaType;
    std::vector<v_frame_t> m_Frames;
    std::unordered_map<v_frame_t, Frame> m_Entries;

private: /* Methods */
    /**
     * Resets underlying map to now start housing the entries represented by the provided first entry
     */
    void Reset(Frame& frame, const MediaType& type);

    void UpdateRange();
    void ProcessMovie();
};

class VOID_API MediaFS
{
public:
    inline MediaStruct GetMediaStruct(const std::string& filepath) { return MediaStruct::FromFile(filepath); }

    /**
     * @brief Returns a path which has the template resolved from the path
     * e.g. if the path provided is /path/to/media/name.####.exr then the path returned would be
     * /path/to/media/name.1001.exr where 1001 could be one of the frames of the media on disk
     * the internal MediaStruct conversion does not care about what frame is provided, as long as it's on the same path
     * 
     * If the provided path is not templated, then the same shall be returned.
     * 
     * @param filepath Path to the media, could be templated (i.e. having #s or printf formatting) or not.
     * @return std::string Path to a numbered frame media file on disk. Returns the same path, if it is not templated.
     */
    static std::string ResolvedPath(const std::string& filepath);

    /**
     * Provides a vector of MediaStructs for a provided Directory
     */
    std::vector<MediaStruct> FromDirectory(const std::string& directory);
    static std::vector<MediaStruct> GetAllMedia(const std::string& directory, int level = 0, int maxLevel = 4);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_FILE_SYSTEM_H
