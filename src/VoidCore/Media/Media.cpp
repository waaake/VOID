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
    , m_Type(Type::UNDEFINED)
    , m_Caching(false)
    , m_StopCaching(false)
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
    if (m_View.framenumbers.empty())
        return;

    /* Sort the Updated frames vector */
    std::sort(m_View.framenumbers.begin(), m_View.framenumbers.end());

    /* Update the first and last frame after the framenumbers have been sorted */
    m_FirstFrame = m_View.framenumbers.front();
    m_LastFrame = m_View.framenumbers.back();
}

v_frame_t Media::NearestFrame(const v_frame_t frame) const
{
    /* We need the lower bound of the given frame available in the vector */
    auto it = std::lower_bound(m_View.framenumbers.begin(), m_View.framenumbers.end(), frame);

    if (it != m_View.framenumbers.begin())
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
    m_View.media = mstruct;

    /**
     * Check if we have a Plugin reader for the file format
     * If not -> we can't proceed
     */
    if (!Forge::Instance().IsRegistered(m_View.media.Extension()))
    {
        VOID_LOG_WARN("No Media Reader found for type {0}", m_View.media.Extension());
        return;
    }

    m_Views.reserve(1);
    m_Views.emplace_back(m_View);

    /* If it is a movie -> process it as one */
    m_View.media.Type() == MediaType::Movie ? ProcessMovie() : ProcessSequence();
}

void Media::Read(MediaStruct&& mstruct)
{
    m_View.media = std::move(mstruct);

    /**
     * Check if we have a Plugin reader for the file format
     * If not -> we can't proceed
     */
    if (!Forge::Instance().IsRegistered(m_View.media.Extension()))
    {
        VOID_LOG_WARN("No Media Reader found for type {0}", m_View.media.Extension());
        return;
    }

    m_Views.reserve(1);
    m_Views.emplace_back(m_View);

    /* If it is a movie -> process it as one */
    m_View.media.Type() == MediaType::Movie ? ProcessMovie() : ProcessSequence();
}

void Media::ProcessSequence()
{
    m_View.framenumbers.reserve(m_View.media.Size());

    /* Iterate over the struct's internal Media Entry */
    for (const MEntry& e: m_View.media)
    {
        /* Update internal structures with the frame information */
        m_View.frames[e.Framenumber()] = std::move(Frame(e));
        m_View.framenumbers.emplace_back(e.Framenumber());
    }

    m_Type = Media::Type::IMAGE_SEQUENCE;
    UpdateRange();
}

// void Media::ProcessMovie(const MediaStruct& mstruct)
void Media::ProcessMovie()
{
    /* Get the First entry since it is a Single File Movie */
    MEntry entry = m_View.media.First();

    if (!entry.Valid())
        return;

    /* Media Reader */
    std::unique_ptr<VoidMPixReader> r = Forge::Instance().GetMovieReader(m_View.media.Extension(), entry.Fullpath());

    MFrameRange frange = r->Framerange();
    VOID_LOG_INFO("Movie Media Range: {0}-{1}", frange.startframe, frange.endframe);

    /* Update internal framerate */
    m_Framerate = r->Framerate();

    m_View.framenumbers.reserve(frange.endframe - frange.startframe + 1);

    /* Add each of the Frame with the same entry and the varying frame number */
    for (v_frame_t i = frange.startframe; i < frange.endframe; i++)
    {
        /* Update internal structures with the frame information */
        m_View.frames[i] = std::move(MovieFrame(entry, i));
        m_View.framenumbers.emplace_back(i);
    }

    m_Type = Media::Type::MOVIE;
    UpdateRange();
}

void Media::Cache()
{
    /* Update the Caching state */
    m_Caching = true;

    /* For all the frames in the media frames */
    for (std::pair<const v_frame_t, Frame>& it: m_View.frames)
    {
        /* Check if we're supposed to stop caching frames further */
        if (m_StopCaching)
        {
            /* Set the state of stopCaching back to 0 so that we don't unnecessarily stop future caches */
            m_StopCaching = false;
            break;
        }

        /* Cache the Frame */
        it.second.Cache();
    }

    /* We're done with the caching */
    m_Caching = false;
}

void Media::ClearCache()
{
    /* For all the frames in the media frames */
    for (std::pair<const v_frame_t, Frame>& it: m_View.frames)
    {
        /* Clear the Data for the Frame from the memory */
        it.second.ClearCache();
    }
}

void Media::StopCaching()
{
    /*
     * Check if we're caching currently
     * if so -> set the var to allow the caching process to stop at the current frame
     * and not proceed to the other frames
     */
    if (m_Caching)
    {
        m_StopCaching = false;
        VOID_LOG_INFO("Stopping Cache");
    }
    VOID_LOG_INFO("No ongoing Cache Process");
}

VOID_NAMESPACE_CLOSE
