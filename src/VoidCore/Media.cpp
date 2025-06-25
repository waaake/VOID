/* STD */
#include <algorithm>

/* Internal */
#include "Media.h"

VOID_NAMESPACE_OPEN

Frame::Frame()
{
    m_ImageData = nullptr;
}

Frame::Frame(const MEntry& e)
    : Frame(e, e.Framenumber())
{
}

Frame::Frame(const MEntry& e, int frame)
    : m_MediaEntry(e)
    , m_Framenumber(frame)
{
    /**
     * Since we have the Media Entry, we can now get the PixReader for the media type
     */
    m_ImageData = std::move(Forge::Instance().GetImageReader(m_MediaEntry.Extension()));
}

Frame::~Frame()
{
    // if (m_ImageData)
    // {
    //     delete m_ImageData;
    //     m_ImageData = nullptr;
    // }
}

SharedPixels Frame::Image()
{
    /*
     * If the frame data has not yet been fetched
     * Read the frame data and return the pointer to the data
     */
    Cache();

    return m_ImageData;
}

void Frame::Cache()
{
    /* Read and load the image data onto the memory */
    if (m_ImageData->Empty())
    {
        m_ImageData->Read(m_MediaEntry.Fullpath(), m_Framenumber);
        VOID_LOG_INFO("Cached Frame: {0}", m_MediaEntry.Framenumber());
    }
}

void Frame::ClearCache()
{
    if (!m_ImageData->Empty())
    {
        m_ImageData->Clear();
        VOID_LOG_INFO("Cleared Frame Cache: {0}", m_MediaEntry.Framenumber());
    }
}

Media::Media()
    : m_FirstFrame(-1)
    , m_LastFrame(-1)
    , m_Type(Type::UNDEFINED)
    , m_Caching(false)
    , m_StopCaching(false)
{
}

Media::Media(const MediaStruct& mstruct)
{
    Read(mstruct);
}

Media::~Media()
{
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

int Media::NearestFrame(const int frame) const
{
    /* We need the lower bound of the given frame available in the vector */
    auto it = std::lower_bound(m_Framenumbers.begin(), m_Framenumbers.end(), frame);

    if (it != m_Framenumbers.end())
    {
        /* Return the value at the iter after moving it back */
        return *(--it);
    }

    /* 
     * As the provided frame is lower than the first frame,
     * The most natural nearest frame to it is the first frame
     */
    return m_FirstFrame;
}

void Media::Read(const MediaStruct& mstruct)
{
    /* Update the underlying struct */
    m_MediaStruct = mstruct;

    /**
     * Check if we have a Plugin reader for the file format
     * If not -> we can't proceed
     */
    if (!Forge::Instance().GetImageReader(mstruct.Extension()))
    {
        VOID_LOG_WARN("No Media Reader found for type {0}", mstruct.Extension());
        return;
    }

    /* If it is a movie -> process it as one */
    if (mstruct.Type() == MediaType::Movie)
    {
        return ProcessMovie(mstruct);
    }

    /* Iterate over the struct's internal Media Entry */
    for (const MEntry& e: mstruct)
    {
        /* Update internal structures with the frame information */
        m_Mediaframes[e.Framenumber()] = Frame(e);
        m_Framenumbers.push_back(e.Framenumber());
    }

    /*
     * Update the frame range as we have read any of the media present in the given path
     */
    UpdateRange();
}

void Media::ProcessMovie(const MediaStruct& mstruct)
{

}

void Media::Cache()
{
    /* Update the Caching state */
    m_Caching = true;

    /* For all the frames in the media frames */
    for (std::pair<int, Frame> it: m_Mediaframes)
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
    for (std::pair<int, Frame> it: m_Mediaframes)
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
