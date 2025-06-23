/* STD */
#include <algorithm>

/* Internal */
#include "Media.h"

VOID_NAMESPACE_OPEN

ImageFrame::ImageFrame()
{
    m_Path = "";
    m_Name = "";
    m_Extension = "";
    m_Framenumber = 0;
}

ImageFrame::ImageFrame(const std::string& path)
{
    /* The read path */
    m_Path = path;

    /* Update All information from the Image path */
    Update();
}

ImageFrame::~ImageFrame()
{
    // if (m_Pixels)
    // {
    //     delete m_Pixels;
    // }
}

bool ImageFrame::ValidFrame(const std::string& framestring) const
{
    /* Nothing to check */
    if (framestring.empty())
        return false;

    /* Check each of the character of the string */
    for (char c: framestring)
    {
        if (!std::isdigit(c))
            return false;
    }

    /* Valid frame */
    return true;
}

void ImageFrame::Update()
{
    std::filesystem::path filepath(m_Path);

    std::string filename = filepath.filename().string();

    size_t lastDot = filename.find_last_of(".");
    std::string remaining = filename.substr(0, lastDot);

    m_Extension = filename.substr(lastDot + 1);

    lastDot = remaining.find_last_of(".");
    std::string framestring = remaining.substr(lastDot + 1);

    /* Image is a sequence or atleast follows a sequential naming convention */
    if (ValidFrame(framestring))
    {
        m_Framenumber = std::stoi(framestring);
        m_Name = remaining.substr(0, lastDot);
    }
    else /* In case we don't have an image sequence, just a standard image */
    {
        m_Name = remaining;
    }
}

SharedPixBlock ImageFrame::Image()
{
    /* If we haven't */
    if (!m_Pixels || m_Pixels && m_Pixels->Empty())
        Cache();

    return m_Pixels;
}

void ImageFrame::Cache()
{
    /**
     * Get the Image Reader for the extension
     * At this point we always should have our Image reader for the extension
     */
    std::unique_ptr<ImageReader> reader = Forge::Instance().GetImageReader(m_Extension);

    if (!reader)
        VOID_LOG_ERROR("Cannot Determine a Reader for the extension {0}.", m_Extension);

    /* Read the data from the Image path */
    m_Pixels = reader->Read(m_Path);
}

void ImageFrame::ClearCache()
{
    /* Clear any data from the PixelBlock */
    if (m_Pixels)
    {
        /* Clear any pixel data */
        m_Pixels->Clear();
    }
}

/* }}} */

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

std::string Media::FileExtension(const std::filesystem::path& path)
{
    std::string ext = path.extension();
    /* Check if the extension exists and it has a dot */
    return (!ext.empty() && ext[0] == '.') ? ext.substr(1) : ext;
}

void Media::Read(const std::string& path)
{
    std::chrono::time_point start = std::chrono::high_resolution_clock::now();

    /* Update the base path */
    m_Path = path;

    Forge& forge = Forge::Instance();

    try
    {
        for (std::filesystem::directory_entry entry: std::filesystem::directory_iterator(path))
        {
            // entry.path().extension()
            // Frame f(entry.path().string());
            std::string ext = FileExtension(entry.path());
            VOID_LOG_INFO(ext);

            /*
             * If it is a Movie
             * Process it like a movie where we can read it's frame information using FFMPEG API
             * Else Continue adding frames from the current directory_entry iterator
             */
            if (forge.IsMovie(ext))
            {
                ProcessMovie(entry.path());
                break;
            }

            /*
             * Check if the frame is a sequence or an image
             * If it is not an image at this point, meaning this file is something we can ignore
             * And proceed checking on other files
             */
            if (!forge.IsImage(ext))
                continue;

            /* Create an image frame from the path */
            ImageFrame f(entry.path());

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

    /*
     * Update the frame range as we have read any of the media present in the given path
     */
    UpdateRange();

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
    for (std::pair<const int, ImageFrame> it: m_Mediaframes)
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
    for (std::pair<const int, ImageFrame> it: m_Mediaframes)
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
