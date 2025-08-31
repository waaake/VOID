// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_MEDIA_CLIP_H
#define _VOID_MEDIA_CLIP_H

/* STD */
#include <memory>
#include <unordered_map>

/* Qt */
#include <QColor>
#include <QPixmap>

/* Internal */
#include "Definition.h"
#include "VoidCore/Media.h"
#include "VoidRenderer/RenderTypes.h"
#include "VoidObjects/VoidObject.h"

VOID_NAMESPACE_OPEN

/* Forward Declaration for typedef */
class MediaClip;
typedef std::shared_ptr<MediaClip> SharedMediaClip;

class VOID_API MediaClip : public VoidObject, public Media
{
    Q_OBJECT

public:
    MediaClip(QObject* parent = nullptr);
    MediaClip(const MediaStruct& mstruct, QObject* parent = nullptr);
    MediaClip(const std::string& basepath,
            const std::string& name,
            const std::string& extension,
            QObject* parent = nullptr
    );
    MediaClip(const std::string& basepath,
            const std::string& name,
            const std::string& extension,
            v_frame_t start,
            v_frame_t end,
            unsigned int padding,
            QObject* parent = nullptr
    );
    MediaClip(const std::string& basepath,
            const std::string& name,
            const std::string& extension,
            v_frame_t start,
            v_frame_t end,
            unsigned int padding,
            const std::vector<v_frame_t>& missing,
            QObject* parent = nullptr
    );
    virtual ~MediaClip();

    inline void SetColor(const QColor& color)
    {
        m_Color = color;
        /* A change has been made */
        emit updated();
    }

    inline QColor Color() const { return m_Color; }

    /* Overloading Base Media functions to allow frameCached be emit */
    inline SharedPixels Image(v_frame_t frame, bool cached = true)
    { 
        /* Emit that the frame was cached */
        emit frameCached(frame);

        /* Return the frame data from media */
        return Media::Image(frame, cached);
    }

    /* 
     * Caches all the frames for the media
     * emits frameCached for all the frames as they are cached
     * Overloads the function Cache from media
     */
    void Cache()
    {
        /* For each frame in Media -> Cache the frame and emit the signal that a frame has been cached */
        for (std::pair<const v_frame_t, Frame>& it: m_Mediaframes)
        {
            /* Cache the data for the frame */
            it.second.Cache();

            /* Emit the frame which was cached */
            emit frameCached(it.first);
        }
    }

    void CacheFrame(v_frame_t frame);
    void UncacheFrame(v_frame_t frame);

    inline size_t FrameSize() { return Media::Image(m_FirstFrame)->FrameSize(); }

    /* Add Annotation for a Frame */
    void SetAnnotation(const v_frame_t frame, const Renderer::SharedAnnotation& annotation);
    /* Remove Anotation for the frame */
    void RemoveAnnotation(const v_frame_t frame);

    /**
     * Returns the Annotation for the frame 
     * nullptr if the annotation isn't found
     */
    Renderer::SharedAnnotation Annotation(const v_frame_t frame) const;
    inline v_frame_t Duration() const { return (m_LastFrame - m_FirstFrame) + 1; }

    QPixmap Thumbnail();

    inline SharedPixels FirstImage() { return Image(FirstFrame()); }
    inline SharedPixels LastImage() { return Image(LastFrame()); }

    void Serialize(rapidjson::Value& out, rapidjson::Document::AllocatorType& allocator) const override;
    void Deserialize(const rapidjson::Value& in) override;

    const char* TypeName() const override { return "Media"; }

signals: /* Signals defining any change that has happened */
    /*
     * Defines if the media or any entity internally has been updated
     * This is an intimation for other entities relying on this to update themselves
     */
    void updated();

    /*
     * Emitted when a frame is cached
     * The cache could happen when the media cache operation is run continuously on a thread
     * Or if the frame is queried by the viewport
     */
    void frameCached(v_frame_t frame);
    void frameUncached(v_frame_t frame);

private: /* Members */
    QColor m_Color;
    QPixmap m_Thumbnail;

    /**
     * This struct saves the Annotations for the frames
     * Each Shared Annotation Pointer is mapped to a frame (as simple as it can be :D)
     */
    std::unordered_map<v_frame_t, Renderer::SharedAnnotation> m_Annotations;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_CLIP_H
