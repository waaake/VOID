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
    , m_Framerate(24.0)
    , m_Samplerate(0)
    , m_AudioChannels(0)
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


void Media::UpdateRange()
{
    /* Check if we have any frames to arrange */
    if (m_Framenumbers.empty())
        return;

    /* Sort the Updated frames vector */
    std::sort(m_Framenumbers.begin(), m_Framenumbers.end());

    /* Update the first and last frame after the framenumbers have been sorted */
    m_FirstFrame = m_Framenumbers.front();
    m_LastFrame = m_Framenumbers.back();
}

v_frame_t Media::NearestFrame(const v_frame_t frame) const
{
    /* We need the lower bound of the given frame available in the vector */
    auto it = std::lower_bound(m_Framenumbers.begin(), m_Framenumbers.end(), frame);

    if (it != m_Framenumbers.begin())
    {
        /* Return the value at the iter after moving it back */
        return *(--it);
    }

    /**
     * As the provided frame is lower than the first frame,
     * The most natural nearest frame to it is the first frame
     */
    return m_FirstFrame;
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
    m_Framenumbers.reserve(m_MediaStruct.Size());

    /* Iterate over the struct's internal Media Entry */
    for (const MEntry& e: m_MediaStruct)
    {
        /* Update internal structures with the frame information */
        m_Mediaframes[e.Framenumber()] = std::move(Frame(e));
        m_Framenumbers.emplace_back(e.Framenumber());
    }

    m_Type = Media::Type::IMAGE_SEQUENCE;
    UpdateRange();
}

// void Media::ProcessMovie(const MediaStruct& mstruct)
void Media::ProcessMovie()
{
    /* Get the First entry since it is a Single File Movie */
    MEntry entry = m_MediaStruct.First();

    if (!entry.Valid())
        return;

    /* Media Reader */
    std::unique_ptr<VoidMPixReader> r = Forge::Instance().GetMovieReader(m_MediaStruct.Extension(), entry.Fullpath());

    MFrameRange frange = r->Framerange();
    VOID_LOG_INFO("Movie Media Range: {0}-{1}", frange.startframe, frange.endframe);

    /* Update internal framerate */
    m_Framerate = r->Framerate();
    m_AudioChannels = r->AudioChannels();
    m_Samplerate = r->Samplerate();

    VOID_LOG_INFO("Movie Audio Details: Samplerate: {0}, Channels: {1}", m_Samplerate, m_AudioChannels);

    m_Framenumbers.reserve(frange.endframe - frange.startframe + 1);

    /* Add each of the Frame with the same entry and the varying frame number */
    for (v_frame_t i = frange.startframe; i < frange.endframe; i++)
    {
        /* Update internal structures with the frame information */
        m_Mediaframes[i] = std::move(MovieFrame(entry, i));
        m_Framenumbers.emplace_back(i);
    }

    m_Type = Media::Type::MOVIE;
    UpdateRange();
}


void Media::ClearCache()
{
    /* For all the frames in the media frames */
    for (std::pair<const v_frame_t, Frame>& it: m_Mediaframes)
    {
        /* Clear the Data for the Frame from the memory */
        it.second.ClearCache();
    }
}


VOID_NAMESPACE_CLOSE
