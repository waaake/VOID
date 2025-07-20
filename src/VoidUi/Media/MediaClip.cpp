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
{
    VOID_LOG_INFO("Clip Created: {0}", Vuid());
}

MediaClip::~MediaClip()
{
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
