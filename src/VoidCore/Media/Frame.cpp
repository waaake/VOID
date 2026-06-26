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
    m_ImageData = nullptr;
}

Frame::Frame(const MEntry& e)
    : Frame(e, e.Framenumber())
{
}

Frame::Frame(const MEntry& e, v_frame_t frame)
    : m_MediaEntry(e)
    , m_Framenumber(frame)
    , m_Writable(nullptr)
{
    // // Since we have the Media Entry, we can now get the PixReader for the media type
    // m_ImageData = std::move(Forge::Instance().GetImageReader(
    //     m_MediaEntry.Extension(),
    //     m_MediaEntry.Fullpath(),
    //     m_Framenumber
    // ));
}

Frame::~Frame()
{
    // if (m_ImageData)
    // {
    //     delete m_ImageData;
    //     m_ImageData = nullptr;
    // }
}

Frame::Frame(Frame&& other) noexcept
    : m_MediaEntry(std::move(other.m_MediaEntry))
    , m_Framenumber(other.m_Framenumber)
    , m_ImageData(std::move(other.m_ImageData))
{
}

Frame& Frame::operator=(Frame&& other) noexcept
{
    if (&other == this)
        return *this;

    m_MediaEntry = std::move(other.m_MediaEntry);
    m_Framenumber = other.m_Framenumber;
    m_ImageData = std::move(other.m_ImageData);

    return *this;
}

Frame::Frame(const Frame& other)
    : m_MediaEntry(other.m_MediaEntry)
    , m_ImageData(other.m_ImageData)
    , m_Framenumber(other.m_Framenumber)
{
}

Frame& Frame::operator=(const Frame& other)
{
    // if (this != &other)
    // {
    //     m_MediaEntry = other.m_MediaEntry;
    //     m_ImageData = other.m_ImageData;
    //     m_Framenumber = other.m_Framenumber;
    // }

    return *this;
}

SharedPixReader Frame::Image(bool cached)
{
    // /*
    //  * If the frame data has not yet been fetched
    //  * Read the frame data and return the pointer to the data
    //  */
    // if (cached)
    //     Cache();

    // return m_Writable && !m_Writable->Empty() ? m_Writable : m_ImageData;
    return nullptr;
}

SharedPixReader Frame::Writable()
{
    // // Ensure we have the image already read, else we can't have a valid writable copy
    // Cache();

    // // Coming from the fact that everytime the Image buffer is copied into the m_Writable shared pointer
    // // We're continuously allocating and deallocating huge amounts of data
    // // instead here we allocate only once, the first time when the writable buffer does not exist
    // // next time onwards, just copy the underlying original image data onto the writable buffer
    // // save us from the extra memory alloc-dealloc
    // if (m_Writable && m_Writable->FrameSize() == m_ImageData->FrameSize())
    //     std::memcpy(m_Writable->Writable(), m_ImageData->Pixels(), m_ImageData->FrameSize());
    // else
    //     m_Writable = m_ImageData->Copy();

    // // m_Writable = m_ImageData->Copy();
    // return m_Writable;
    return nullptr;
}

void Frame::Cache()
{
    // /**
    //  * Don't allow mutliple threads to cache the same frame
    //  * if one thread has the file open, that should be then allowed to be closed
    //  * before the other thread tries to read it again, this comes from caching frame
    //  * with threads as there could be a possiblitity that the main thread has reached
    //  * this frame and has requested the frame data where as the frame isn't fully ready yet
    //  * which is then checking whether the image data is empty, it could be partially filled
    //  * allowing the conditional check to fail and render a partially read frame, or worse case
    //  * try to open the file again and result in unexpected behaviour including malloc or free related
    //  * crashes
    //  */
    // std::lock_guard<std::mutex> guard(m_Mutex);
    // if (m_ImageData->Empty())
    // {
    //     m_ImageData->Read();
    //     m_Channels = m_ImageData->Channels();
    // }
}

void Frame::ClearCache(bool dirty)
{
    // if (!m_ImageData->Empty())
    // {
    //     // Don't allow concurrent access when clearing the underlying data vector
    //     std::lock_guard<std::mutex> guard(m_Mutex);
    //     m_ImageData->Clear();
    // }
    // if (m_Writable && !m_Writable->Empty())
    // {
    //     // Don't allow concurrent access when clearing the underlying data vector
    //     std::lock_guard<std::mutex> guard(m_Mutex);
    //     m_Writable->Clear();
    // }
    // m_Dirty = dirty;
}

MovieFrame::MovieFrame(const MEntry& e, const v_frame_t frame)
{
    // /* Update the entry */
    // m_MediaEntry = e;
    // /* The frame */
    // m_Framenumber = frame;

    // /**
    //  * And get the Reader
    //  * As of today there isn't a usecase to hold the movie reader in the Movie frame
    //  * as a SharedMPixReader as all the bits about reading are same
    //  */
    // m_ImageData = std::move(Forge::Instance().GetMovieReader(
    //     m_MediaEntry.Extension(),
    //     m_MediaEntry.Fullpath(),
    //     m_Framenumber
    // ));
}

VOID_NAMESPACE_CLOSE
