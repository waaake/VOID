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
#include "View.h"
#include "VoidCore/MediaFilesystem.h"

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
    inline std::string Path() const { return m_View.media.Basepath(); }
    inline std::string Name() const { return m_View.media.Name(); }
    inline std::string Extension() const { return m_View.media.Extension(); }

    inline Media::Type MediaType() const { return m_Type; }

    // inline bool Valid() const { return m_Type != Type::NON_MEDIA; }

    inline v_frame_t FirstFrame() const { return m_FirstFrame; }
    inline v_frame_t LastFrame() const { return m_LastFrame; }

    inline v_frame_t Duration() const { return (m_LastFrame - m_FirstFrame) + 1; }

    /*
     * Returns whether a given frame falls in the range of Media
     * i.e. between the first and the last frame of media
     * Any frame missing does not matter as this method only returns whether a frame is in the range or not
     */
    [[nodiscard]] inline bool HasFrame(v_frame_t frame) const { return frame >= m_FirstFrame && frame <= m_LastFrame; }

    /* 
     * Returns whether a given frame is available to read
     * There could be a scenario where the given frame is in the range of first - last but is not available
     * and is referred to as the missing frame.
     */
    [[nodiscard]] inline bool Contains(v_frame_t frame) const { return m_View.Contains(frame); }

    /*
     * Based on the available frames, returns the frame which is just lower than the provided frame
     * This is used when the current frame is not available but we want the neartest frame to be used in it's place
     */
    v_frame_t NearestFrame(v_frame_t frame) const;

    Frame GetFrame(v_frame_t frame) const { return m_View.GetFrame(frame); }
    Frame FirstFrameData() const { return m_View.frames.at(FirstFrame()); }
    Frame LastFrameData() const { return m_View.frames.at(LastFrame()); }

    inline SharedPixels Image(v_frame_t frame, bool cached = true) { return m_View.frames.at(frame).Image(cached); }

    inline SharedPixels FirstImage() { return Image(FirstFrame()); }
    inline SharedPixels LastImage() { return Image(LastFrame()); }

    inline const std::map<std::string, std::string> Metadata() const { return m_View.frames.at(m_FirstFrame).Metadata(); }

    inline double Framerate() const { return m_Framerate; }
    inline bool Empty() const { return m_View.Empty(); }
    /*
     * A Media can be considered invalid if it is empty
     * Any valid media will have atleast one frame
     */
    inline bool Valid() const { return !Empty(); }

    inline void Clear()
    {
        for (View& view : m_Views)
            view.Clear();
    }

    /*
     * Caches all frames of the Media onto memory
     * This method is CPU intensive function and should be called from a separate thread
     * to allow this function to run parallelly to other elements
     */
    void Cache();

    /*
     * Clears the cache for all the frames of the Media
     */
    void ClearCache();

    /* Stops caching if the cache process is ongoing */
    void StopCaching();

    /* Returns whether the Cache process is ongoing */
    inline bool Caching() const { return m_StopCaching; }

    /* Allow iterating over the Media frames */
    inline std::unordered_map<v_frame_t, Frame>::iterator begin() { return m_View.frames.begin(); }
    inline std::unordered_map<v_frame_t, Frame>::iterator end() { return m_View.frames.end(); }

protected: /* Members */
    v_frame_t m_FirstFrame, m_LastFrame;
    double m_Framerate;
    Type m_Type;

    /* State determining that caching is currently ongoing */
    bool m_Caching;
    /*
     * This variable controls when the caching operation needs to be stopped
     * If set to True and if the caching process was active
     */
    bool m_StopCaching;
    View m_View;
    Views m_Views;

private: /* Methods */
    void ProcessSequence();
    void ProcessMovie();

    /* Updates the internal range based on the read frames */
    void UpdateRange();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_H
