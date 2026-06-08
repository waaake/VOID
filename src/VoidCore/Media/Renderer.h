// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _MEDIA_RENDERER_H
#define _MEDIA_RENDERER_H

/* Internal */
#include "Definition.h"
#include "Filesystem.h"
#include "PixWriter.h"

VOID_NAMESPACE_OPEN

namespace Renderer {

class VOID_API ImageRenderer
{
public:
    ImageRenderer(const MEntry& entry, const EncodeSpec& spec);
    ~ImageRenderer();

    bool Valid() const { return (bool)m_Writer; }

    bool AddBuffer(const void* buf, std::size_t size, const InputSpec& spec);
    bool AddBuffer(v_frame_t frame, const void* buffer, std::size_t size, const InputSpec& spec);

    bool Render();
    bool Render(const void* buffer, std::size_t size, const InputSpec& spec);
    bool Render(v_frame_t frame, const void* buffer, std::size_t size, const InputSpec& spec);

private:
    std::unique_ptr<PixWriter> m_Writer;
    MEntry m_MediaEntry;
};

class VOID_API MovieRenderer
{
public:
    MovieRenderer(const MEntry& entry, const EncodeSpec& spec);
    ~MovieRenderer();

    bool Valid() const { return (bool)m_Writer; }

    bool AddBuffer(const void* buf, std::size_t size, const InputSpec& spec);
    bool Render();

private:
    std::unique_ptr<PixWriter> m_Writer;
    MEntry m_MediaEntry;
};

} // namespace Renderer

VOID_NAMESPACE_CLOSE

#endif // _MEDIA_RENDERER_H
