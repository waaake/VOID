// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_MEDIA_FRAME_H
#define _VOID_MEDIA_FRAME_H

/* STD */
#include <mutex>

/* Internal */
#include "Definition.h"
#include "PixReader.h"
#include "VoidCore/MediaFilesystem.h"

VOID_NAMESPACE_OPEN

/**
 * @brief Frame describes a Frame of Media
 * The frame could point to an actual physical file on disk, in case of an image sequence
 * or it could be a frame in Memory for a Movie Media, that describe the contents of the Movie
 * at a given Presentation Time Stamp (PTS) which will be rendered on screen when requested
 * 
 */
class VOID_API Frame
{
public:
    Frame();
    explicit Frame(const MEntry& e);
    Frame(const MEntry& e, v_frame_t frame);

    ~Frame();

    /* Move */
    Frame(Frame&& other) noexcept;
    Frame& operator=(Frame&& other) noexcept;

    /* Copy */
    Frame(const Frame& other);
    Frame& operator=(const Frame& other);

    /* Getters */
    inline const std::string& Path() const { return m_MediaEntry.Fullpath(); }
    inline const std::string& Name() const { return m_MediaEntry.Name(); }
    inline const std::string& Extension() const { return m_MediaEntry.Extension(); }
    inline v_frame_t Framenumber() const { return m_Framenumber; }

    /**
     * Returns Shared Pointer to the ImageData
     * cached defines whether the frame needs to be read before returning
     * has no effect if the frame has already been read
     */
    SharedPixels Image(bool cached = true);

    /**
     * Returns the underlying metadata from the image
     */
    inline const std::map<std::string, std::string> Metadata() const { return m_ImageData->Metadata(); }

    /* Frame Caches */
    void Cache();
    void ClearCache();

protected: /* Members */
    MEntry m_MediaEntry;
    SharedPixels m_ImageData;

    /* Internally associated framenumer */
    v_frame_t m_Framenumber;

private: /* Members*/
    std::mutex m_Mutex;
};

class VOID_API MovieFrame : public Frame
{
public:
    MovieFrame() : Frame() {}
    MovieFrame(const MEntry& e, const v_frame_t frame);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_FRAME_H
