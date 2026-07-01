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
    // if (m_Reader)
    // {
    //     delete m_Reader;
    //     m_Reader = nullptr;
    // }
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

// SharedPixReader Frame::Image(bool cached)
// {
//     // /*
//     //  * If the frame data has not yet been fetched
//     //  * Read the frame data and return the pointer to the data
//     //  */
//     // if (cached)
//     //     Cache();

//     // return m_Writable && !m_Writable->Empty() ? m_Writable : m_Reader;
//     return nullptr;
// }

// SharedPixReader Frame::Writable()
// {
//     // // Ensure we have the image already read, else we can't have a valid writable copy
//     // Cache();

//     // // Coming from the fact that everytime the Image buffer is copied into the m_Writable shared pointer
//     // // We're continuously allocating and deallocating huge amounts of data
//     // // instead here we allocate only once, the first time when the writable buffer does not exist
//     // // next time onwards, just copy the underlying original image data onto the writable buffer
//     // // save us from the extra memory alloc-dealloc
//     // if (m_Writable && m_Writable->FrameSize() == m_Reader->FrameSize())
//     //     std::memcpy(m_Writable->Writable(), m_Reader->Pixels(), m_Reader->FrameSize());
//     // else
//     //     m_Writable = m_Reader->Copy();

//     // // m_Writable = m_Reader->Copy();
//     // return m_Writable;
//     return nullptr;
// }

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
    // if (m_Writable && !m_Writable->Empty())
    // {
    //     // Don't allow concurrent access when clearing the underlying data vector
    //     std::lock_guard<std::mutex> guard(m_Mutex);
    //     m_Writable->Clear();
    // }
    m_Dirty = dirty;
}

MovieFrame::MovieFrame(const MEntry& e, const v_frame_t frame)
{
    m_Entry = e;
    m_Framenumber = frame;

    m_Reader = std::move(Forge::Instance().MovieReaderAt(m_Entry.Extension(), m_Entry.Fullpath(), m_Framenumber));

    // /* Update the entry */
    // m_Entry = e;
    // /* The frame */
    // m_Framenumber = frame;

    // /**
    //  * And get the Reader
    //  * As of today there isn't a usecase to hold the movie reader in the Movie frame
    //  * as a SharedMPixReader as all the bits about reading are same
    //  */
    // m_Reader = std::move(Forge::Instance().GetMovieReader(
    //     m_Entry.Extension(),
    //     m_Entry.Fullpath(),
    //     m_Framenumber
    // ));
}

VOID_NAMESPACE_CLOSE
