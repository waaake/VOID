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
    // m_BasePath = filepath.parent_path().string();

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
    const std::string* it = std::find(std::begin(m_MovieTypes), std::end(m_MovieTypes), m_Extension);

    /* true if the extension is in set of movie types */
    return it != std::end(m_MovieTypes);
}

VoidImageData* Frame::ImageData()
{
    /*
     * If the frame data has not yet been fetched
     * Read the frame data and return the pointer to the data
     */
    if (m_ImageData->Empty())
        m_ImageData->Read(m_Path);

    return m_ImageData;
}

Media::Media()
    : m_Path("")
    , m_Extension("")
    , m_Name("")
    , m_FirstFrame(-1)
    , m_LastFrame(-1)
    , m_Type(Type::UNDEFINED)
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

VOID_NAMESPACE_CLOSE
