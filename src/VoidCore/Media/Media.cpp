// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <algorithm>

/* Internal */
#include "FormatForge.h"
#include "Media.h"
#include "VoidCore/Logging.h"
#include "VoidCore/Profiler.h"

VOID_NAMESPACE_OPEN

Media::Media()
    : m_FirstFrame(-1)
    , m_LastFrame(-1)
    , m_Samplerate(0.0)
    , m_Framerate(24.0)
    , m_Type(Type::UNDEFINED)
{
}

Media::Media(const MediaStruct& mstruct)
    : Media()
{
    Read(mstruct);
}

Media::Media(MediaStruct& mstruct)
    : Media()
{
    Read(std::move(mstruct));
}

Media::~Media()
{
}

Media::Media(const std::string& basepath, const std::string& name, const std::string& extension)
    : Media()
{
    Read(MediaStruct(basepath, name, extension));
}

Media::Media(const std::string& basepath,
        const std::string& name,
        const std::string& extension,
        v_frame_t start,
        v_frame_t end,
        unsigned int padding
    )
    : Media()
{
    Read(MediaStruct(basepath, name, extension, start, end, padding));
}

Media::Media(const std::string& basepath,
        const std::string& name,
        const std::string& extension,
        v_frame_t start,
        v_frame_t end,
        unsigned int padding,
        const std::vector<v_frame_t>& missing
    )
    : Media()
{
    Read(MediaStruct(basepath, name, extension, start, end, padding, missing));
}

v_frame_t Media::NearestFrame(const v_frame_t frame) const
{
    /**
     * find a frame just next to given frame which exists in the set of frames that the media has
     * if it exists, return that else we return that else the first frame from the media frames
     */
    auto it = std::lower_bound(m_Framenumbers.begin(), m_Framenumbers.end(), frame);
    return (it == m_Framenumbers.begin()) ? m_FirstFrame : *(--it);
}

bool Media::Contains(v_frame_t frame) const
{
    /**
     * A movie would have all the frames in it, even if something is technially missing,
     * movie still has still information about those, only an image sequence needs to be checked
     * for actual frame being present even if it's still in the range 
     */
    return m_Type == Media::Type::MOVIE ? true : InRange(frame) && GetFrame(frame).Valid();
}

void Media::Read(const MediaStruct& mstruct)
{
    m_MediaStruct = mstruct;

    /**
     * Check if we have a Plugin reader for the file format
     * If not -> we can't proceed
     */
    if (!Forge::Instance().IsRegistered(m_MediaStruct.Extension()))
    {
        VOID_LOG_WARN("No Media Reader found for type {0}", m_MediaStruct.Extension());
        return;
    }

    /* If it is a movie -> process it as one */
    m_MediaStruct.Type() == MediaType::Movie ? ProcessMovie() : ProcessSequence();
}

void Media::Read(MediaStruct&& mstruct)
{
    m_MediaStruct = std::move(mstruct);

    /**
     * Check if we have a Plugin reader for the file format
     * If not -> we can't proceed
     */
    if (!Forge::Instance().IsRegistered(m_MediaStruct.Extension()))
    {
        VOID_LOG_WARN("No Media Reader found for type {0}", m_MediaStruct.Extension());
        return;
    }

    /* If it is a movie -> process it as one */
    m_MediaStruct.Type() == MediaType::Movie ? ProcessMovie() : ProcessSequence();
}

void Media::ProcessSequence()
{
    const MFrameRange& frange = m_MediaStruct.Framerange();
    m_FirstFrame = frange.startframe;
    m_LastFrame = frange.endframe;

    m_Framenumbers.reserve(frange.duration);
    m_Mediaframes.resize(frange.duration);

    for (const MEntry& e : m_MediaStruct)
    {
        /**
         * Index here is basically the distance between the current frame being processed
         * and the start frame, this is how we actually want the data to always be layed out
         * meaning that the frames are always ordered and correspond to the distance between the frames
         * 
         * e.g. if we need frame 1010 and the start frame is 1001, we know that the index to look at
         * will be 1010 - 1001 = 9
         */
        const int index = e.Framenumber() - m_FirstFrame;

        m_Mediaframes[index] = std::move(Frame(e));
        m_Framenumbers.emplace_back(e.Framenumber());
    }

    m_Type = Media::Type::IMAGE_SEQUENCE;
}

void Media::ProcessMovie()
{
    /* Get the First entry since it is a Single File Movie */
    MEntry entry = m_MediaStruct.First();

    if (!entry.Valid())
        return;

    /* Media Reader */
    std::unique_ptr<VoidMPixReader> r = Forge::Instance().GetMovieReader(m_MediaStruct.Extension(), entry.Fullpath());

    MFrameRange frange = r->Framerange();
    VOID_LOG_INFO("Movie Media Range: {0}-{1}--{2}", frange.startframe, frange.endframe, frange.duration);

    /* Update internal framerate */
    m_Framerate = r->Framerate();
    m_Samplerate = r->Samplerate();

    m_Mediaframes.resize(frange.duration);
    m_Framenumbers.reserve(frange.duration);

    m_FirstFrame = frange.startframe;
    m_LastFrame = frange.endframe;

    for (v_frame_t i = frange.startframe; i < frange.duration; ++i)
    {
        m_Mediaframes[i] = std::move(MovieFrame(entry, i));
        m_Framenumbers.emplace_back(i);
    }

    m_Type = Media::Type::MOVIE;
}

void Media::ClearCache()
{
    for (Frame& f : m_Mediaframes)
        f.ClearCache();
}

Frame Media::GetFrame(v_frame_t frame) const
{
    /**
     * Same logic as mentioned before, the frames in the underlying vector are always sorted
     * and point to the index same as the distance between the frames
     * 
     * e.g. if we need frame 1010 and the start frame is 1001, we know that the index to look at
     * will be 1010 - 1001 = 9
     */
    return m_Mediaframes.at(frame - m_FirstFrame);
}

VOID_NAMESPACE_CLOSE
