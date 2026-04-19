// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_MEDIA_CLIP_H
#define _VOID_MEDIA_CLIP_H

/* STD */
#include <memory>
#include <unordered_map>
#include <vector>

/* Qt */
#include <QColor>
#include <QPixmap>
#include <QRunnable>

/* Internal */
#include "Definition.h"
#include "VoidCore/Media/Media.h"
#include "VoidObjects/VoidObject.h"
#include "VoidObjects/Models/TagModel.h"
#include "VoidRenderer/Core/RenderTypes.h"

VOID_NAMESPACE_OPEN

/* Forward Declaration for typedef */
class MediaClip;
typedef std::shared_ptr<MediaClip> SharedMediaClip;

class Effect;

class VOID_API MediaClip : public VoidObject, public Media
{
    Q_OBJECT

public:
    MediaClip(QObject* parent = nullptr);
    MediaClip(const MediaStruct& mstruct, QObject* parent = nullptr);
    MediaClip(MediaStruct& mstruct, QObject* parent = nullptr);
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

    void CacheFrame(v_frame_t frame);
    void UncacheFrame(v_frame_t frame);

    /* Add Annotation for a Frame */
    void SetAnnotation(const v_frame_t frame, const Renderer::SharedAnnotation& annotation);
    /* Remove Anotation for the frame */
    void RemoveAnnotation(const v_frame_t frame);

    void AddTag(const std::string& name);
    void AddTag(const std::string& name, TagMetadataModel*& metadata);
    void ClearTags();

    Effect* AddEffect(const std::string& type);
    void ClearEffects();

    inline const std::vector<Tag*>& Tags() const { return m_TagModel->Tags(); }
    inline bool HasTags() const { return m_TagModel->HasTags(); }
    inline TagModel* TagsModel() const { return m_TagModel; }

    /**
     * Returns the Annotation for the frame 
     * nullptr if the annotation isn't found
     */
    Renderer::SharedAnnotation Annotation(const v_frame_t frame) const;
    std::vector<int> AnnotatedFrames() const;

    QPixmap Thumbnail();

    void Serialize(rapidjson::Value& out, rapidjson::Document::AllocatorType& allocator) const override;
    void Serialize(std::ostream& out) const override;

    void Deserialize(const rapidjson::Value& in) override;
    void Deserialize(std::istream& in) override;

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
    TagModel* m_TagModel;

    /**
     * This struct saves the Annotations for the frames
     * Each Shared Annotation Pointer is mapped to a frame (as simple as it can be :D)
     */
    std::unordered_map<v_frame_t, Renderer::SharedAnnotation> m_Annotations;
    std::vector<Effect*> m_Effects;

private: /* Methods */
    void ReadThumbnail();
    QPixmap DefaultThumbnail();

private: /* Classes */
    class MediaThumbnailCacheRunner : public QRunnable
    {
    public:
        MediaThumbnailCacheRunner(MediaClip* clip) : m_Clip(clip) {}
        inline void run() override
        {
            if (m_Clip)
                m_Clip->ReadThumbnail();
        }

    private:
        MediaClip* m_Clip;
    };
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_CLIP_H
