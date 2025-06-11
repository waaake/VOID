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
}

MediaClip::~MediaClip()
{
}

VOID_NAMESPACE_CLOSE
