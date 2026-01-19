// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <filesystem>
#include <regex>

/* Internal */
#include "Frame.h"
#include "FormatForge.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

/* MHelper {{{ */

MediaType MHelper::GetMediaType(const Frame& entry)
{
    /* If it's a Movie */
    if (IsMovie(entry))
        return MediaType::Movie;
    else if (IsImage(entry))
        return MediaType::Image;

    /**
     * For Now Audio isn't interpreted
     * TODO: Add Audio format detection
     */

    return MediaType::NonMedia;
}

/* }}} */

Frame::Frame()
    : m_Path("")
    , m_Basepath("")
    , m_Name("")
    , m_Extension("")
    , m_Framenumber(0)
    , m_FramePadding(0)
    , m_SingleFile(false)
    , m_ImageData(nullptr)
{
}

Frame::Frame(const std::string& path)
    : Frame()
{
    /* Parse the provided path and update internal token */
    Parse(path);
}

Frame::Frame(const std::string& basepath, const std::string& name, const std::string& extension, v_frame_t frame, unsigned int padding, bool singlefile)
    : m_Basepath(basepath)
    , m_Name(name)
    , m_Extension(extension)
    , m_Framenumber(frame)
    , m_FramePadding(padding)
    , m_SingleFile(singlefile)
    , m_ImageData(nullptr)
{
    std::filesystem::path path = std::filesystem::path(m_Basepath) / name;
    if (!singlefile)
        path += "." + PaddedFrame(frame);

    path += "." + extension;
    m_Path = path.string();
}

Frame::~Frame()
{
}

Frame::Frame(Frame&& other) noexcept
    : m_Path(other.m_Path)
    , m_Basepath(other.m_Basepath)
    , m_Name(other.m_Name)
    , m_Extension(other.m_Extension)
    , m_FramePadding(other.m_FramePadding)
    , m_Framenumber(other.m_Framenumber)
    , m_SingleFile(other.m_SingleFile)
    , m_Templated(other.m_Templated)
    , m_ImageData(other.m_ImageData)
{
}

Frame& Frame::operator=(Frame&& other) noexcept
{
    if (&other == this)
        return *this;
    
    m_Path = other.m_Path;
    m_Basepath = other.m_Basepath;
    m_Name = other.m_Name;
    m_Extension = other.m_Extension;
    m_FramePadding = other.m_FramePadding;
    m_Framenumber = other.m_Framenumber;
    m_SingleFile = other.m_SingleFile;
    m_Templated = other.m_Templated;
    m_ImageData = other.m_ImageData;

    return *this;
}

Frame::Frame(const Frame& other)
    : m_Path(other.m_Path)
    , m_Basepath(other.m_Basepath)
    , m_Name(other.m_Name)
    , m_Extension(other.m_Extension)
    , m_FramePadding(other.m_FramePadding)
    , m_Framenumber(other.m_Framenumber)
    , m_SingleFile(other.m_SingleFile)
    , m_Templated(other.m_Templated)
    , m_ImageData(other.m_ImageData)
{
}

Frame& Frame::operator=(const Frame& other)
{
    if (&other == this)
        return *this;

    m_Path = other.m_Path;
    m_Basepath = other.m_Basepath;
    m_Name = other.m_Name;
    m_Extension = other.m_Extension;
    m_FramePadding = other.m_FramePadding;
    m_Framenumber = other.m_Framenumber;
    m_SingleFile = other.m_SingleFile;
    m_Templated = other.m_Templated;
    m_ImageData = other.m_ImageData;

    return *this;
}

void Frame::Parse(const std::string& path)
{
    /* Update the fullpath */
    m_Path = path;

    std::filesystem::path filepath(path);

    /* The base path for the file */
    m_Basepath = filepath.parent_path().string();
    /* Grab the filename from the rest of the base path */
    std::string filename = filepath.filename().string();

    /* From the last . try to get the extension and other parts */
    size_t lastDot = filename.find_last_of(".");
    std::string remaining = filename.substr(0, lastDot);

    m_Extension = filename.substr(lastDot + 1);

    lastDot = remaining.find_last_of(".");
    std::string framestring = remaining.substr(lastDot + 1);

    /* Validate a case where file is named 1.ext or 1001.ext */
    if (framestring == remaining)
    {
        m_Name = remaining;
        /* It can be considered as a single file */
        m_SingleFile = true;

        /* No need to proceed further */
        return;
    }

    /**
     * Image is a sequence or atleast follows a sequential naming convention
     * Check if the framestring is a valid number before trying to cast that to one
     */
    if (ValidFrame(framestring))
    {
        m_FramePadding = framestring.size();
        m_Framenumber = std::stol(framestring);
        m_Name = remaining.substr(0, lastDot);
    }
    else if (ValidTemplate(framestring))
    {
        m_Templated = true;
        m_Name = remaining.substr(0, lastDot);
    }
    else /* In case we don't have an image sequence, just a standard image */
    {
        m_Name = remaining;
        /* Update the state so we know it is a single file */
        m_SingleFile = true;
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

bool Frame::ValidTemplate(const std::string& framestring) const
{
    std::regex hashPattern("(#+)");
    std::regex printfPattern("%0\\d+d");

    return std::regex_search(framestring, hashPattern) || std::regex_search(framestring, printfPattern);
}

std::string Frame::PaddedFrame(v_frame_t frame) const
{
    std::stringstream ss;
    ss << std::setw(m_FramePadding) << std::setfill('0') << frame;
    return ss.str();
}

void Frame::Setup()
{
    m_ImageData = std::move(Forge::Instance().GetImageReader(m_Extension, m_Path, m_Framenumber));
}

void Frame::Setup(v_frame_t framenumber)
{
    m_Framenumber = framenumber;
    m_ImageData = std::move(Forge::Instance().GetMovieReader(m_Extension, m_Path, m_Framenumber));
}

SharedPixels Frame::Image(bool cached)
{
    /*
     * If the frame data has not yet been fetched
     * Read the frame data and return the pointer to the data
     */
    if (cached)
        Cache();

    return m_ImageData;
}

void Frame::Cache()
{
    /**
     * Don't allow mutliple threads to cache the same frame
     * if one thread has the file open, that should be then allowed to be closed
     * before the other thread tries to read it again, this comes from caching frame
     * with threads as there could be a possiblitity that the main thread has reached
     * this frame and has requested the frame data where as the frame isn't fully ready yet
     * which is then checking whether the image data is empty, it could be partially filled
     * allowing the conditional check to fail and render a partially read frame, or worse case
     * try to open the file again and result in unexpected behaviour including malloc or free related
     * crashes
     */
    std::lock_guard<std::mutex> guard(m_Mutex);

    /* Read and load the image data onto the memory */
    if (m_ImageData->Empty())
        m_ImageData->Read();
}

void Frame::ClearCache()
{
    if (!m_ImageData->Empty())
    {
        /**
         * Don't allow concurrent access when clearing the underlying data vector
         */
        std::lock_guard<std::mutex> guard(m_Mutex);
        m_ImageData->Clear();
    }
}

VOID_NAMESPACE_CLOSE
