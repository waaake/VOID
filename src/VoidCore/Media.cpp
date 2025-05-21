/* STD */
#include <algorithm>

/* Internal */
#include "Media.h"

VOID_NAMESPACE_OPEN

Frame::Frame()
    : m_Path("")
    , m_Name("")
    , m_Extension("")
    , m_Framenum(0)
{
    m_ImageData = new VoidImageData;
}

Frame::Frame(const std::string& path)
{
    m_ImageData = new VoidImageData;
    Read(path);
}

Frame::~Frame()
{
}

void Frame::Read(const std::string& path)
{
    m_Path = path;

    std::filesystem::path filepath(path);

    std::string filename = filepath.filename().string();

    size_t lastDot = filename.find_last_of(".");
    std::string remaining = filename.substr(0, lastDot);

    m_Extension = filename.substr(lastDot + 1);

    lastDot = remaining.find_last_of(".");
    std::string framestring = remaining.substr(lastDot + 1);

    /* Image is a sequence or atleast follows a sequential naming convention */
    if (ValidFrame(framestring))
    {
        m_Framenum = std::stoi(framestring);
        m_Name = remaining.substr(0, lastDot);
    }
    else /* In case we don't have an image sequence, just a standard image */
    {
        m_Name = remaining;
    }
}

bool Frame::ValidFrame(const std::string& framestring) const
{
    if (framestring.empty())
        return false;

    for (char c: framestring)
    {
        if (!std::isdigit(c))
            return false;
    }

    /* Valid frame */
    return true;
}

bool Frame::IsMovie() const
{
    const std::string* it = std::find(std::begin(m_MovieFormats), std::end(m_MovieFormats), m_Extension);

    /* true if the extension is in set of movie types */
    return it != std::end(m_MovieFormats);
}

bool Frame::IsImage() const
{
    const std::string* it = std::find(std::begin(m_ImageFormats), std::end(m_ImageFormats), m_Extension);

    /* true if the extension is in set of Image types */
    return it != std::end(m_ImageFormats);
}

VoidImageData* Frame::ImageData()
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
        m_ImageData->Read(m_Path);
        VOID_LOG_INFO("Cached Frame: {0}", m_Framenum);
    }
}

void Frame::ClearCache()
{
    if (!m_ImageData->Empty())
    {
        m_ImageData->Free();
        VOID_LOG_INFO("Cleared Frame Cache: {0}", m_Framenum);
    }
}

Media::Media()
    : m_Path("")
    , m_Extension("")
    , m_Name("")
    , m_FirstFrame(-1)
    , m_LastFrame(-1)
    , m_Type(Type::UNDEFINED)
    , m_Caching(false)
    , m_StopCaching(false)
{
}

Media::Media(const std::string& path)
{
    Read(path);
}

Media::~Media()
{
}

int Media::FirstFrame() const
{
    std::vector<int>::const_iterator it = std::min_element(m_Framenumbers.begin(), m_Framenumbers.end());

    /* Return the minimum value from the framenumbers set */
    if (it != m_Framenumbers.end())
    {
        return *it;
    }

    /* No frames to process */
    return -1;
}

int Media::LastFrame() const
{
    std::vector<int>::const_iterator it = std::max_element(m_Framenumbers.begin(), m_Framenumbers.end());

    /* Return the maximum value from the framenumbers set */
    if (it != m_Framenumbers.end())
    {
        return *it;
    }

    /* No frames to process */
    return -1;
}

void Media::Read(const std::string& path)
{
    std::chrono::time_point start = std::chrono::high_resolution_clock::now();

    /* Update the base path */
    m_Path = path;

    try
    {
        for (std::filesystem::directory_entry entry: std::filesystem::directory_iterator(path))
        {
            Frame f(entry.path().string());

            /*
             * If it is a Movie
             * Process it like a movie where we can read it's frame information using FFMPEG API
             * Else Continue adding frames from the current directory_entry iterator
             */
            if (f.IsMovie())
            {
                ProcessMovie(f.Path());
                break;
            }
            
            /* 
             * Check if the frame is a sequence or an image
             * If it is not an image at this point, meaning this file is something we can ignore
             * And proceed checking on other files
             */
            if (!f.IsImage())
                continue;

            /* Update internal structures with the frame information */
            m_Mediaframes[f.Framenumber()] = f;
            m_Framenumbers.push_back(f.Framenumber());
        }
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        VOID_LOG_ERROR(e.what());
    }

    std::chrono::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    VOID_LOG_INFO("Time Taken to Load : {0}", duration.count());
}

void Media::ProcessMovie(const std::string& path)
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
