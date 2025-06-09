/* Internal */
#include "MediaClip.h"

VOID_NAMESPACE_OPEN

MediaClip::MediaClip(QObject* parent)
    : QObject(parent)
{
}

MediaClip::MediaClip(const Media& media, QObject* parent)
    : QObject(parent)
    , m_Media(media)
{
}

MediaClip::~MediaClip()
{
}

VOID_NAMESPACE_CLOSE
