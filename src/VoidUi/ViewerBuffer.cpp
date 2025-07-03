/* Internal */
#include "ViewerBuffer.h"

VOID_NAMESPACE_OPEN

ViewerBuffer::ViewerBuffer(const std::string& name, QObject* parent)
    : QObject(parent)
    , m_Clip(std::make_shared<MediaClip>())
    , m_Track(std::make_shared<PlaybackTrack>())
    , m_Sequence(std::make_shared<PlaybackSequence>())
    , m_PlayingComponent(PlayableComponent::Clip)
    , m_Startframe(0)
    , m_Endframe(100)
    , m_Name(name)
    , m_Color(130, 110, 190)    // Purple
    , m_Active(false)
{
}

ViewerBuffer::ViewerBuffer(QObject* parent)
    : ViewerBuffer("", parent)
{
}

ViewerBuffer::~ViewerBuffer()
{
}

void ViewerBuffer::Set(const SharedMediaClip& media)
{
    /* Update the internal media clip */
    m_Clip = media;

    /* Update the color on the Media to indicate that it belongs to this buffer */
    m_Clip->SetColor(m_Color);

    /* Update the current Playing Component */
    m_PlayingComponent = PlayableComponent::Clip;

    /* Update frame range */
    m_Startframe = m_Clip->FirstFrame();
    m_Endframe = m_Clip->LastFrame();

    /* Refresh to clear any underlying caches */
    Refresh();
}

void ViewerBuffer::Set(const SharedPlaybackTrack& track)
{
    /* Update the internal track */
    m_Track = track;

    /* Update the color on the Track to indicate that it belongs to this buffer */
    m_Track->SetColor(m_Color);

    /* Update the current Playing component */
    m_PlayingComponent = PlayableComponent::Track;

    /* Update frame range */
    m_Startframe = m_Track->StartFrame();
    m_Endframe = m_Track->EndFrame();

    /* Refresh to clear any underlying caches */
    Refresh();
}

void ViewerBuffer::Set(const SharedPlaybackSequence& sequence)
{
    /* Update the internal sequence */
    m_Sequence = sequence;

    /* Update the current Playing Component */
    m_PlayingComponent = PlayableComponent::Sequence;

    /* Update frame range */
    m_Startframe = m_Sequence->StartFrame();
    m_Endframe = m_Sequence->EndFrame();

    /* Refresh to clear any underlying caches */
    Refresh();
}

void ViewerBuffer::SetColor(const QColor& color)
{
    /* Update the Buffer Color */
    m_Color = color;

    /* Update entities with the color change */
    m_Clip->SetColor(color);
    m_Track->SetColor(color);
}

SharedPlaybackTrack ViewerBuffer::ActiveTrack() const
{
    switch (m_PlayingComponent)
    {
        case PlayableComponent::Clip:
            /**
             * A clip is a separate entity and does not relate to track
             * If a clip is active then it should be the one currently playing and no track
             */
            return nullptr;
        case PlayableComponent::Track:
            /**
             * If the current playing component itself is a track, then it will be considered
             * as the active track, this could involve in adding elements to the active track
             */
            return m_Track;
        case PlayableComponent::Sequence:
            /**
             * If a sequence is currently playing, then this returns the currently active track
             * Meaning the track which is on the very top and also which is both enabled and visible
             * for it to recieve new entities and also show entities
             */
            /**
             * TODO: Need to think about, if this is really needed when right clicking > add media to sequence?
             * if added, where does the media go to? at the last of track or clears it?
             */
            return m_Sequence->ActiveVideoTrack();
    }

    return nullptr;
}

void ViewerBuffer::Refresh()
{
    /* Clear any cached item */
    m_CachedTrackItem = nullptr;
}

void ViewerBuffer::ClearCache()
{
    /* Clear Cache from the playing components */
    if (m_Clip)
        m_Clip->ClearCache();

    if (m_Track)
        m_Track->ClearCache();
}

SharedTrackItem ViewerBuffer::ItemFromSequence(const int frame)
{
    /**
     * Check if we have a cached track item present -> return that
     * if the cached track item is not preset or if the item does not have the requested frame
     * request for a track item from the track at the frame, cache it and return back
     */
    if (!m_CachedTrackItem || !m_CachedTrackItem->HasFrame(frame))
    {
        /* Cache the item from the Sequence */
        m_CachedTrackItem = m_Sequence->GetTrackItem(frame);
    }

    /* Return what has been cached */
    return m_CachedTrackItem;
}

SharedTrackItem ViewerBuffer::ItemFromTrack(const int frame)
{
    /**
     * Check if we have a cached track item present -> return that
     * if the cached track item is not preset or if the item does not have the requested frame
     * request for a track item from the track at the frame, cache it and return back
     */
    if (!m_CachedTrackItem || !m_CachedTrackItem->HasFrame(frame))
    {
        /* Cache the item from the track */
        m_CachedTrackItem = m_Track->GetTrackItem(frame);
    }

    /* Return what has been cached */
    return m_CachedTrackItem;
}

void ViewerBuffer::Clear()
{
    /* Clear up --> Point to an empty clip */
    m_Clip = std::make_shared<MediaClip>();
    /* Clear Cache */
    ClearCache();
}

bool ViewerBuffer::Playing(const SharedMediaClip& media) const
{
    /* If a clip is being played check if the internal clip and the media are same */
    if (m_PlayingComponent == PlayableComponent::Clip)
        return media == m_Clip;

    /**
     * In other cases the cached track item should tell us if the clip was being played currently
     */
    return m_CachedTrackItem->GetMedia() == media;
}

VOID_NAMESPACE_CLOSE
