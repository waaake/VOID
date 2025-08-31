// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPixmap>

/* Internal */
#include "MediaClip.h"

VOID_NAMESPACE_OPEN

MediaClip::MediaClip(QObject* parent)
    : VoidObject(parent)
    , Media()
{
    VOID_LOG_INFO("Clip Created: {0}", Vuid());
}

MediaClip::MediaClip(const MediaStruct& mstruct, QObject* parent)
    : VoidObject(parent)
    , Media(mstruct)
    , m_Thumbnail()
{
    VOID_LOG_INFO("Clip Created: {0}", Vuid());
}

MediaClip::MediaClip(const std::string& basepath,
        const std::string& name,
        const std::string& extension,
        QObject* parent
    )
    : VoidObject(parent)
    , Media(basepath, name, extension)
    , m_Thumbnail()
{
    VOID_LOG_INFO("Clip Created: {0}", Vuid());
}

MediaClip::MediaClip(const std::string& basepath,
        const std::string& name,
        const std::string& extension,
        v_frame_t start,
        v_frame_t end,
        unsigned int padding,
        QObject* parent
    )
    : VoidObject(parent)
    , Media(basepath, name, extension, start, end, padding)
    , m_Thumbnail()
{
    VOID_LOG_INFO("Clip Created: {0}", Vuid());
}

MediaClip::MediaClip(const std::string& basepath,
        const std::string& name,
        const std::string& extension,
        v_frame_t start,
        v_frame_t end,
        unsigned int padding,
        const std::vector<v_frame_t>& missing,
        QObject* parent
    )
    : VoidObject(parent)
    , Media(basepath, name, extension, start, end, padding, missing)
    , m_Thumbnail()
{
    VOID_LOG_INFO("Clip Created: {0}", Vuid());
}


MediaClip::~MediaClip()
{
}

QPixmap MediaClip::Thumbnail()
{
    if (m_Thumbnail.isNull())
    {
        /* Grab the pointer to the image data for the first frame to be used as a thumbnail */
        SharedPixels im = Media::FirstImage();
        QImage::Format format = (im->Channels() == 3) ? QImage::Format_RGB888 : QImage::Format_RGBA8888;

        m_Thumbnail = QPixmap::fromImage(QImage(im->ThumbnailPixels(), im->Width(), im->Height(), format)).scaledToWidth(400, Qt::SmoothTransformation);
        /* Clear the data for when required */
        im->Clear();
    }

    return m_Thumbnail;
}

Renderer::SharedAnnotation MediaClip::Annotation(const v_frame_t frame) const
{
    /* We have an annotation available for the given frame */
    if (m_Annotations.find(frame) != m_Annotations.end())
        return m_Annotations.at(frame);

    /* Nothing available */
    return nullptr;
}

/* Add Annotation for a Frame */
void MediaClip::SetAnnotation(const v_frame_t frame, const Renderer::SharedAnnotation& annotation)
{
    /* Update the Annotation */
    m_Annotations[frame] = annotation;
    /* Media Clip has been updated */
    emit updated();

    VOID_LOG_INFO("Annotation Added. Frame {0}", frame);
}

void MediaClip::RemoveAnnotation(const v_frame_t frame)
{ 
    /**
     * Remove Annotation at frame
     * Since the map holds a shared pointer to the Annotation, which should get
     * deleted as there are no more references to it
     */
    m_Annotations.erase(frame);
    /* Media clip has been updated */
    emit updated();

    VOID_LOG_INFO("Annotation Removed. Frame {0}", frame);
}

void MediaClip::CacheFrame(v_frame_t frame)
{
    m_Mediaframes.at(frame).Cache();
    // emit frameCached(frame);
}

void MediaClip::UncacheFrame(v_frame_t frame)
{
    m_Mediaframes.at(frame).ClearCache();
    // emit frameUncached(frame);
}

void MediaClip::Serialize(rapidjson::Value& out, rapidjson::Document::AllocatorType& allocator) const
{
    out.SetObject();

    out.AddMember("type", rapidjson::Value(TypeName(), allocator), allocator);
    out.AddMember("basepath", rapidjson::Value(m_MediaStruct.Basepath().c_str(), allocator), allocator);
    out.AddMember("name", rapidjson::Value(m_MediaStruct.Name().c_str(), allocator), allocator);
    out.AddMember("extension", rapidjson::Value(m_MediaStruct.Extension().c_str(), allocator), allocator);
    out.AddMember("start", static_cast<int64_t>(m_FirstFrame), allocator);
    out.AddMember("end", static_cast<int64_t>(m_LastFrame), allocator);
    out.AddMember("singlefile", static_cast<int>(m_MediaStruct.SingleFile()), allocator);
    out.AddMember("framePadding", static_cast<unsigned int>(m_MediaStruct.Framepadding()), allocator);
}

void MediaClip::Deserialize(const rapidjson::Value& in)
{
    /* A Single file is just something like a single image or maybe an audio and a movie like like .mp4, .mov etc. */
    if (in["singlefile"].GetInt())
    {
        Read(
            MediaStruct(
                in["basepath"].GetString(),
                in["name"].GetString(),
                in["extension"].GetString()
            )
        );
    }
    else
    {
        Read(
            MediaStruct(
                in["basepath"].GetString(),
                in["name"].GetString(),
                in["extension"].GetString(),
                in["start"].GetInt64(),
                in["end"].GetInt64(),
                in["framePadding"].GetUint()
            )
        );
    }
}

VOID_NAMESPACE_CLOSE
