// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPixmap>

/* Internal */
#include "MediaClip.h"

VOID_NAMESPACE_OPEN

MediaClip::MediaClip(QObject* parent)
    : VoidObject(parent)
{
    VOID_LOG_INFO("Clip Created: {0}", Vuid());
}

MediaClip::MediaClip(const Media& media, QObject* parent)
    : VoidObject(parent)
    , m_Media(media)
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
        const SharedPixels im = m_Media.FirstImage();
        QImage::Format format = (im->Channels() == 3) ? QImage::Format_RGB888 : QImage::Format_RGBA8888;

        m_Thumbnail = QPixmap::fromImage(QImage(im->ThumbnailPixels(), im->Width(), im->Height(), format));
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

VOID_NAMESPACE_CLOSE
