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
    inline std::string Fullpath() const { return m_MediaStruct.FirstPath(); }
    inline std::string Path() const { return m_MediaStruct.Basepath(); }
    inline std::string Name() const { return m_MediaStruct.Name(); }
    inline std::string Extension() const { return m_MediaStruct.Extension(); }

    inline Media::Type MediaType() const { return m_Type; }
    inline bool HasAudio() const { return bool(m_Samplerate); }

    // inline bool Valid() const { return m_Type != Type::NON_MEDIA; }

    inline v_frame_t FirstFrame() const { return m_FirstFrame; }
    inline v_frame_t LastFrame() const { return m_LastFrame; }

    inline v_frame_t Duration() const { return (m_LastFrame - m_FirstFrame) + 1; }

    /**
     * Returns whether a given frame falls in the range of Media
     * i.e. between the first and the last frame of media
     * Any frame missing does not matter as this method only returns whether a frame is in the range or not
     */
    [[nodiscard]] inline bool InRange(v_frame_t frame) const { return frame >= m_FirstFrame && frame <= m_LastFrame; }

    /** 
     * Returns whether a given frame is available to read
     * There could be a scenario where the given frame is in the range of first - last but is not available
     * and is referred to as the missing frame.
     */
    [[nodiscard]] bool Contains(v_frame_t frame) const;

    /**
     * Based on the available frames, returns the frame which is just lower than the provided frame
     * This is used when the current frame is not available but we want the neartest frame to be used in it's place
     */
    v_frame_t NearestFrame(v_frame_t frame) const;

    inline Frame GetFrame(v_frame_t frame) const { return m_Mediaframes.at(frame - m_FirstFrame); }
    inline Frame FirstFrameData() const { return GetFrame(m_FirstFrame); }
    inline Frame LastFrameData() const { return GetFrame(m_LastFrame); }

    inline SharedPixels Image(v_frame_t frame, bool cached = true) { return GetFrame(frame).Image(cached); }
    std::size_t FrameSize();

    inline SharedPixels FirstImage() { return Image(m_FirstFrame); }
    inline SharedPixels LastImage() { return Image(m_LastFrame); }

    inline const std::map<std::string, std::string> Metadata() const { return GetFrame(m_FirstFrame).Metadata(); }

    inline double Framerate() const { return m_Framerate; }
    inline bool Empty() const { return m_Mediaframes.empty(); }
    /**
     * A Media can be considered invalid if it is empty
     * Any valid media will have atleast one frame
     */
    inline bool Valid() const { return !Empty(); }
    void Clear();

    /**
     * Clears the cache for all the frames of the Media
     */
    void ClearCache();

    /* Allow iterating over the Media frames */
    inline std::vector<Frame>::iterator begin() { return m_Mediaframes.begin(); }
    inline std::vector<Frame>::iterator end() { return m_Mediaframes.end(); }

protected: /* Members */
    MediaStruct m_MediaStruct;

    v_frame_t m_FirstFrame, m_LastFrame;
    int m_Samplerate;
    double m_Framerate;
    std::size_t m_Framesize = {0};

    Type m_Type;
    std::vector<Frame> m_Mediaframes;
    std::vector<v_frame_t> m_Framenumbers;

private: /* Methods */
    void ProcessSequence();
    void ProcessMovie();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_H
