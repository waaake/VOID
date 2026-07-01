// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "PlayerBuffer.h"

VOID_NAMESPACE_OPEN

PlayerBuffer::PlayerBuffer(QObject* parent)
    : QObject(parent)
    , m_Clip(std::make_shared<MediaClip>())
    , m_Track(std::make_shared<PlaybackTrack>())
    , m_Sequence(std::make_shared<PlaybackSequence>())
    , m_Playlist(nullptr)
    , m_Startframe(0)
    , m_Endframe(1)
    , m_PlayingComponent(PlayableComponent::Clip)
{
}

void PlayerBuffer::Set(const SharedMediaClip& media)
{
    emit mediaAboutToBeUpdated();

    m_Clip = media;
    m_PlayingComponent = PlayableComponent::Clip;
    UpdateRange(m_Clip->FirstFrame(), m_Clip->LastFrame());

    emit playlistUpdated(nullptr);
    emit mediaUpdated();
}

void PlayerBuffer::Set(const SharedPlaybackTrack& track)
{
    emit mediaAboutToBeUpdated();

    m_Track = track;
    m_PlayingComponent = PlayableComponent::Track;
    UpdateRange(m_Track->StartFrame(), m_Track->EndFrame());

    emit mediaUpdated();
}

void PlayerBuffer::Set(const SharedPlaybackSequence& sequence)
{
    emit mediaAboutToBeUpdated();

    m_Sequence = sequence;
    m_PlayingComponent = PlayableComponent::Sequence;
    UpdateRange(m_Sequence->StartFrame(), m_Sequence->EndFrame());

    emit mediaUpdated();
}

void PlayerBuffer::Set(const std::vector<SharedMediaClip>& media)
{
    emit mediaAboutToBeUpdated();

    m_Track->Clear();
    for (const SharedMediaClip& media : media)
        m_Track->AddMedia(media);

    m_PlayingComponent = PlayableComponent::Track;
    UpdateRange(m_Track->StartFrame(), m_Track->EndFrame());

    emit mediaUpdated();
}

void PlayerBuffer::SetGrid(Playlist* playlist)
{
    emit mediaAboutToBeUpdated();
    if (m_Playlist)
        disconnect(m_Playlist, &Playlist::updated, this, &PlayerBuffer::updated);

    m_Playlist = playlist;
    m_Clip = playlist->CurrentMedia();

    connect(m_Playlist, &Playlist::updated, this, &PlayerBuffer::updated);

    m_PlayingComponent = PlayableComponent::Grid;
    UpdateRange(m_Clip->FirstFrame(), m_Clip->LastFrame());

    emit playlistUpdated(playlist);
    emit mediaUpdated();
}

void PlayerBuffer::SetPlaylist(Playlist* playlist)
{
    // Refresh();
    emit mediaAboutToBeUpdated();
    if (m_Playlist)
        disconnect(m_Playlist, &Playlist::updated, this, &PlayerBuffer::updated);

    m_Playlist = playlist;
    m_Clip = playlist->CurrentMedia();

    connect(m_Playlist, &Playlist::updated, this, &PlayerBuffer::updated);

    m_PlayingComponent = PlayableComponent::Playlist;
    UpdateRange(m_Clip->FirstFrame(), m_Clip->LastFrame());

    emit playlistUpdated(playlist);
    emit mediaUpdated();
}

SharedPlaybackTrack PlayerBuffer::ActiveTrack() const
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
        case PlayableComponent::Grid:
        case PlayableComponent::Playlist:
            return nullptr;
    }

    return nullptr;
}

BufferData PlayerBuffer::MData(const v_frame_t frame, bool nearest)
{
    BufferData d;
    switch (m_PlayingComponent)
    {
        case PlayableComponent::Track:
            d.image = m_Track->Image(frame);
            break;
        case PlayableComponent::Sequence:
            d.image = m_Sequence->Image(frame);
            break;
        default:
            if (m_Clip->Contains(frame))
            {
                d.image = m_Clip->Image(frame);
                d.annotation = m_Clip->Annotation(frame);
            }
    }

    return d;
}

std::vector<FloatImage> PlayerBuffer::GridFrame(const v_frame_t frame)
{
    std::vector<FloatImage> grid;
    grid.reserve(m_Playlist->Size());

    if (m_PlayingComponent == PlayableComponent::Grid)
    {
        for (auto& media : m_Playlist->AllMedia())
        {
            FloatImage image = VOID_NAMESPACE::Image<float>::Create();
            media->Image(media->Contains(frame) ? frame : media->NearestFrame(frame), image);
            grid.push_back(image);
        }
    }

    return grid;
}

SharedMediaClip PlayerBuffer::Media(const v_frame_t frame)
{
    switch (m_PlayingComponent)
    {
        case PlayableComponent::Sequence: return m_Sequence->Media(frame);
        case PlayableComponent::Track: return m_Track->Media(frame);
        case PlayableComponent::Grid:
        case PlayableComponent::Playlist:
        case PlayableComponent::Clip:
        default: return m_Clip;
    }
}

SharedTrackItem PlayerBuffer::TrackItem(const v_frame_t frame)
{
    return (m_PlayingComponent == PlayableComponent::Sequence)
            ? m_Sequence->GetTrackItem(frame)
            : m_Track->GetTrackItem(frame);
}

bool PlayerBuffer::Playing(const SharedMediaClip& media) const
{
    // TODO: Check if we may have a use case around if the clip being compared against be a part of sequence or track as well
    return m_PlayingComponent == PlayableComponent::Clip ? media == m_Clip : false;
}

void PlayerBuffer::SetAnnotation(const v_frame_t frame, const Renderer::SharedAnnotation& annotation)
{
    /**
     * For now Just adding the annotation on the Media Clip
     * TODO: The Active sequence should also have a way to save Annotations to
     * that way it's not applied on to the TrackItem but on the sequence itself (or maybe check behaviours around this)
     */
    if (m_PlayingComponent == PlayableComponent::Clip)
        m_Clip->SetAnnotation(frame, annotation);
}

void PlayerBuffer::RemoveAnnotation(const v_frame_t frame)
{
    /**
     * For now Just removing the annotation from the Media Clip
     * TODO: The Active sequence should also have a way to save Annotations to and remove from
     * that way it's not happening on to TrackItem but on the sequence itself (or maybe check behaviours around this)
     */
    if (m_PlayingComponent == PlayableComponent::Clip)
        m_Clip->RemoveAnnotation(frame);
}

bool PlayerBuffer::NextMedia()
{
    if (m_PlayingComponent == PlayableComponent::Playlist)
    {
        emit mediaAboutToBeUpdated();
        m_Clip = m_Playlist->NextMedia();

        UpdateRange(m_Clip->FirstFrame(), m_Clip->LastFrame());
        emit mediaUpdated();

        return true;
    }

    return false;
}

bool PlayerBuffer::PreviousMedia()
{
    if (m_PlayingComponent == PlayableComponent::Playlist)
    {
        // Refresh();
        emit mediaAboutToBeUpdated();
        m_Clip = m_Playlist->PreviousMedia();

        UpdateRange(m_Clip->FirstFrame(), m_Clip->LastFrame());
        emit mediaUpdated();

        return true;
    }

    return false;
}

bool PlayerBuffer::ResetPlaylistMedia()
{
    /**
     * This is supposed to be invoked when we have reset the current index of the playlist
     * and we want that to be played now,
     * this clears existing cache and fetches the current media info from the playlist if available
     */
    if (m_PlayingComponent == PlayableComponent::Playlist)
    {
        // Refresh();
        emit mediaAboutToBeUpdated();
        m_Clip = m_Playlist->CurrentMedia();

        UpdateRange(m_Clip->FirstFrame(), m_Clip->LastFrame());
        emit mediaUpdated();

        return true;
    }

    return false;
}

void PlayerBuffer::ResetMedia()
{
    m_Clip = std::make_shared<MediaClip>();
}

// int PlayerBuffer::InternalDuration() const
// {
//     // return InternalEndframe() - InternalStartframe() + 1;
//     return m_Endframe - m_Startframe + 1;
// }

void PlayerBuffer::UpdateRange(v_frame_t start, v_frame_t end)
{
    m_Startframe = start;
    m_Endframe = end;

    emit rangeUpdated(start, end);
}

VOID_NAMESPACE_CLOSE
