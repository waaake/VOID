// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <cstring>

/* Internal */
#include "Frame.h"
#include "FormatForge.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

Frame::Frame()
{
    m_Reader = nullptr;
}

Frame::Frame(const MEntry& e)
    : Frame(e, e.Framenumber())
{
}

Frame::Frame(const MEntry& e, v_frame_t frame)
    : m_Entry(e)
    , m_Framenumber(frame)
{
    m_Reader = std::move(Forge::Instance().ImageReaderAt(m_Entry.Extension(), m_Entry.Fullpath(), m_Framenumber));
}

Frame::~Frame()
{
}

Frame::Frame(Frame&& other) noexcept
    : m_Entry(std::move(other.m_Entry))
    , m_Framenumber(other.m_Framenumber)
    , m_Reader(std::move(other.m_Reader))
{
}

Frame& Frame::operator=(Frame&& other) noexcept
{
    if (&other == this)
        return *this;

    m_Entry = std::move(other.m_Entry);
    m_Framenumber = other.m_Framenumber;
    m_Reader = std::move(other.m_Reader);

    return *this;
}

Frame::Frame(const Frame& other)
    : m_Entry(other.m_Entry)
    , m_Reader(other.m_Reader)
    , m_Framenumber(other.m_Framenumber)
{
}

Frame& Frame::operator=(const Frame& other)
{
    if (this != &other)
    {
        m_Entry = other.m_Entry;
        m_Reader = other.m_Reader;
        m_Framenumber = other.m_Framenumber;
    }

    return *this;
}

void Frame::Image(FloatImage& image)
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
    m_Reader->Read(m_Entry.Fullpath(), m_Framenumber, image);
}

const FloatImage& Frame::Image()
{
    Cache();
    return m_Reader->FrameImage();
}

void Frame::Thumbnail(UInt8Image& image)
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
    m_Reader->ReadThumbnail(m_Entry.Fullpath(), m_Framenumber, image);
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
    if (m_Reader->Empty())
        m_Reader->Read();
}

void Frame::Clear(bool dirty)
{
    if (!m_Reader->Empty())
    {
        // Don't allow concurrent access when clearing the underlying data vector
        std::lock_guard<std::mutex> guard(m_Mutex);
        m_Reader->Clear();
    }
    m_Dirty = dirty;
}

MovieFrame::MovieFrame(const MEntry& e, const v_frame_t frame)
{
    m_Entry = e;
    m_Framenumber = frame;

    m_Reader = std::move(Forge::Instance().MovieReaderAt(m_Entry.Extension(), m_Entry.Fullpath(), m_Framenumber));
}

VOID_NAMESPACE_CLOSE
