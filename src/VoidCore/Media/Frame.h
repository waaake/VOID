// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_MEDIA_FRAME_H
#define _VOID_MEDIA_FRAME_H

/* STD */
#include <mutex>

/* Internal */
#include "Definition.h"
#include "PixReader.h"
#include "Filesystem.h"

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

    // Move
    Frame(Frame&& other) noexcept;
    Frame& operator=(Frame&& other) noexcept;

    // Copy
    Frame(const Frame& other);
    Frame& operator=(const Frame& other);

    // Entry Attribs
    inline const std::string& Path() const { return m_Entry.Fullpath(); }
    inline const std::string& Name() const { return m_Entry.Name(); }
    inline const std::string& Extension() const { return m_Entry.Extension(); }
    
    // Frame Attribs
    inline v_frame_t Framenumber() const { return m_Framenumber; }
    inline void SetDirty(bool dirty) { m_Dirty = dirty; }
    [[nodiscard]] inline bool Dirty() const { return m_Dirty; }
    [[nodiscard]] inline bool Invalid() const { return !m_Reader; }
    [[nodiscard]] inline bool Valid() const { return bool(m_Reader); }

    // /**
    //  * Returns Shared Pointer to the ImageData
    //  * cached defines whether the frame needs to be read before returning
    //  * has no effect if the frame has already been read
    //  */
    // SharedPixReader Image(bool cached = true);
    // // SharedPixReader Writable();
    void Thumbnail(UInt8Image& image);
    void Image(FloatImage& image);
    const FloatImage& Image();

    // Image Properties
    [[nodiscard]] inline int Width() const { return m_Reader->Width(); }
    [[nodiscard]] inline int Height() const { return m_Reader->Height(); }
    [[nodiscard]] inline int Channels() const { return m_Reader->Channels(); }
    inline const std::map<std::string, std::string> Metadata() const { return m_Reader->Metadata(); }

    /* Frame Caches */
    void Cache();
    void Clear(bool dirty = true);

protected: /* Members */
    MEntry m_Entry;
    SharedPixReader m_Reader;
    v_frame_t m_Framenumber;
    bool m_Dirty = {false};

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
