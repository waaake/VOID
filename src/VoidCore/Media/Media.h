// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_MEDIA_H
#define _VOID_MEDIA_H

/* STD */
#include <filesystem>
#include <unordered_map>
#include <vector>

/* Internal */
#include "Definition.h"
#include "Frame.h"
#include "Filesystem.h"

VOID_NAMESPACE_OPEN

class VOID_API Media
{
public: /* Enums */
    enum class Type
    {
        UNDEFINED,
        IMAGE_SEQUENCE,
        MOVIE,
        SINGLE_FILE,
        NON_MEDIA
    };

public:
    Media();
    Media(MediaStruct& mstruct);
    Media(const MediaStruct& mstruct);

    /**
     * Constructors that would mostly be used to construct Media from serialized data
     * 1. Singlefile
     * 2. Sequence without missing frames
     * 3. Sequence with missing frames
     */
    Media(const std::string& basepath, const std::string& name, const std::string& extension);

    Media(const std::string& basepath,
        const std::string& name,
        const std::string& extension,
        v_frame_t start,
        v_frame_t end,
        unsigned int padding
    );
    Media(const std::string& basepath,
        const std::string& name,
        const std::string& extension,
        v_frame_t start,
        v_frame_t end,
        unsigned int padding,
        const std::vector<v_frame_t>& missing
    );

    virtual ~Media();

    /**
     * Reads a MediaStructure to update the internals based on the
     * entries in the structure
     */
    void Read(const MediaStruct& mstruct);
    void Read(MediaStruct&& mstruct);

    /* Getters */
    inline std::string Path() const { return m_MediaStruct.Basepath(); }
    inline std::string Name() const { return m_MediaStruct.Name(); }
    inline std::string Extension() const { return m_MediaStruct.Extension(); }
    inline std::string Fullpath() const { return m_MediaStruct.FirstPath(); }

    inline Media::Type MediaType() const { return m_Type; }

    inline v_frame_t FirstFrame() const { return m_MediaStruct.Startframe(); }
    inline v_frame_t LastFrame() const { return m_MediaStruct.Endframe(); }

    inline v_frame_t Duration() const { return m_MediaStruct.Duration(); }

    /*
     * Returns whether a given frame falls in the range of Media
     * i.e. between the first and the last frame of media
     * Any frame missing does not matter as this method only returns whether a frame is in the range or not
     */
    [[nodiscard]] inline bool HasFrame(v_frame_t frame) const { return m_MediaStruct.HasFrame(frame); }

    /* 
     * Returns whether a given frame is available to read
     * There could be a scenario where the given frame is in the range of first - last but is not available
     * and is referred to as the missing frame.
     */
    [[nodiscard]] inline bool Contains(v_frame_t frame) const { return m_MediaStruct.Contains(frame); }

    /*
     * Based on the available frames, returns the frame which is just lower than the provided frame
     * This is used when the current frame is not available but we want the neartest frame to be used in it's place
     */
    inline v_frame_t NearestFrame(v_frame_t frame) const { return m_MediaStruct.NearestFrame(frame); }

    inline SharedPixels Image(v_frame_t frame, bool cached = true) { return m_MediaStruct.Image(frame, cached); }
    inline SharedPixels FirstImage() { return Image(m_MediaStruct.Startframe()); }
    inline SharedPixels LastImage() { return Image(m_MediaStruct.Endframe()); }

    inline size_t FrameSize() { return Image(m_MediaStruct.Startframe())->FrameSize(); }

    inline const std::map<std::string, std::string> Metadata() const { return m_MediaStruct.Metadata(); }

    inline double Framerate() const { return m_MediaStruct.Framerate(); }
    inline bool Empty() const { return m_MediaStruct.Empty(); }
    /*
     * A Media can be considered invalid if it is empty
     * Any valid media will have atleast one frame
     */
    inline bool Valid() const { return !m_MediaStruct.Empty(); }

    inline void Clear() { m_MediaStruct.Clear(); }

    /*
     * Clears the cache for all the frames of the Media
     */
    inline void ClearCache() { m_MediaStruct.ClearCache(); }

    /* Allow iterating over the Media frames */
    inline std::unordered_map<v_frame_t, Frame>::iterator begin() { return m_MediaStruct.begin(); }
    inline std::unordered_map<v_frame_t, Frame>::iterator end() { return m_MediaStruct.end(); }

protected: /* Members */
    /**
     * The Media structure for the Media
     */
    MediaStruct m_MediaStruct;
    Type m_Type;

private: /* Methods */
    void ProcessSequence();
    void ProcessMovie();

    /* Updates the internal range based on the read frames */
    void UpdateRange();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_H
