// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QDragMoveEvent>
#include <QMimeData>

/* Internal */
#include "Player.h"
#include "VoidUi/Descriptors.h"
#include "VoidUi/Media/MediaBridge.h"

VOID_NAMESPACE_OPEN

Player::Player(QWidget* parent)
    : PlayerWidget(parent)
{
    m_CacheProcessor.SetActivePlayer(this);
    Connect();
}

Player::~Player()
{
}

void Player::SetMedia(const SharedMediaClip& media)
{
    /* Reset timeline | Renderer */
    m_Timeline->Clear();
    m_Renderer->Clear();

    /* Update what's currently being played on the viewer buffer */
    m_ActiveViewBuffer->Set(media);
    m_Timeline->SetRange(media->FirstFrame(), media->LastFrame());

    m_CacheProcessor.SetMedia(media);
    m_Timeline->SetAnnotatedFrames(std::move(media->AnnotatedFrames()));

    SetMediaFrame(m_Timeline->Frame());
}

void Player::SetMedia(const std::vector<SharedMediaClip>& media)
{
    m_ActiveViewBuffer->Set(media);
    SetTrack(m_ActiveViewBuffer->ActiveTrack());
}

void Player::SetMedia(const std::vector<SharedMediaClip>& media, const PlayerViewBuffer& buffer)
{
    switch (buffer)
    {
        case PlayerViewBuffer::B:
            m_ViewBufferB->Set(media);
            SetTrack(m_ViewBufferB->ActiveTrack(), buffer);
            break;
        case PlayerViewBuffer::A:
        default:
            m_ViewBufferA->Set(media);
            SetTrack(m_ViewBufferA->ActiveTrack(), buffer);
    }
}

void Player::SetMedia(const SharedMediaClip& media, const PlayerViewBuffer& buffer)
{
    /* Reset timeline | Renderer */
    m_Timeline->Clear();
    m_Renderer->Clear();

    buffer == PlayerViewBuffer::A ? m_ViewBufferA->Set(media) : m_ViewBufferB->Set(media);

    if (Comparing())
        return CompareMediaFrame(m_Timeline->Frame());

    ViewerBuffer *active, *inactive;

    buffer == PlayerViewBuffer::A
        ? (active = m_ViewBufferA, inactive = m_ViewBufferB)
        : (active = m_ViewBufferB, inactive = m_ViewBufferA);

    m_ActiveViewBuffer = active;
    /* Update active states for the buffers */
    active->SetActive(true);
    inactive->SetActive(false);

    /* Refresh the player with the updated content from the Buffer */
    ResetCacheMedia();

    /* Update the frame range */
    SetRange(m_ActiveViewBuffer->StartFrame(), m_ActiveViewBuffer->EndFrame());
    SetMediaFrame(m_Timeline->Frame());
}

void Player::SetTrack(const SharedPlaybackTrack& track)
{
    /* Reset timeline | Renderer */
    m_Timeline->Clear();
    m_Renderer->Clear();

    /* Update what's currently being played on the viewer buffer */
    m_ActiveViewBuffer->Set(track);
    m_Timeline->SetRange(track->StartFrame(), track->EndFrame());
    
    m_CacheProcessor.SetTrack(track);
    SetTrackFrame(m_Timeline->Frame());
}

void Player::SetTrack(const SharedPlaybackTrack& track, const PlayerViewBuffer& buffer)
{
    /* Reset timeline | Renderer */
    m_Timeline->Clear();
    m_Renderer->Clear();

    buffer == PlayerViewBuffer::A ? m_ViewBufferA->Set(track) : m_ViewBufferB->Set(track);

    if (Comparing())
        return CompareMediaFrame(m_Timeline->Frame());

    ViewerBuffer *active, *inactive;

    buffer == PlayerViewBuffer::A
        ? (active = m_ViewBufferA, inactive = m_ViewBufferB)
        : (active = m_ViewBufferB, inactive = m_ViewBufferA);

    m_ActiveViewBuffer = active;
    /* Update active states for the buffers */
    active->SetActive(true);
    inactive->SetActive(false);

    /* Refresh the player with the updated content from the Buffer */
    ResetCacheMedia();
    SetTrackFrame(m_Timeline->Frame());

    /* Update the frame range */
    SetRange(m_ActiveViewBuffer->StartFrame(), m_ActiveViewBuffer->EndFrame());
}

void Player::SetSequence(const SharedPlaybackSequence& sequence)
{
    /* Reset timeline | Renderer */
    m_Timeline->Clear();
    m_Renderer->Clear();

    /* Update what is being played on the Active Viewer Buffer */
    m_ActiveViewBuffer->Set(sequence);
    m_Timeline->SetRange(sequence->StartFrame(), sequence->EndFrame());
    
    m_CacheProcessor.SetSequence(sequence);
    SetSequenceFrame(m_Timeline->Frame());
}

void Player::SetPlaylist(Playlist* playlist)
{
    m_Timeline->Clear();
    m_Renderer->Clear();

    /* Update what is being played on the Active Viewer Buffer */
    m_ActiveViewBuffer->SetPlaylist(playlist);
    SetRange(m_ActiveViewBuffer->StartFrame(), m_ActiveViewBuffer->EndFrame());

    m_CacheProcessor.SetMedia(m_ActiveViewBuffer->GetMediaClip());
    SetMediaFrame(m_Timeline->Frame());
}

void Player::SetFrame(int frame)
{
    /**
     * Comparison Gets precedence --> If we're in comparing mode -> Compare Media frames
     * the media frames could be from any component Track, Clip
     */
    if (Comparing())
        return CompareMediaFrame(frame);

    /**
     * Check what do we want to play
     * if currently playing component on the Active ViewerBuffer is Sequence, meaning that was latest set on it
     */
    if (m_ActiveViewBuffer->PlayingComponent() == ViewerBuffer::PlayableComponent::Sequence)
        return SetSequenceFrame(frame);
    else if (m_ActiveViewBuffer->PlayingComponent() == ViewerBuffer::PlayableComponent::Track)
        return SetTrackFrame(frame);

    return SetMediaFrame(frame);
}

void Player::NextMedia()
{
    if (m_ActiveViewBuffer->NextMedia())
    {
        m_Renderer->Clear();
        ResetCacheMedia();
        m_CacheProcessor.RestartPlaybackCache();

        SetRange(m_ActiveViewBuffer->StartFrame(), m_ActiveViewBuffer->EndFrame());
    }
}

void Player::PreviousMedia()
{
    if (m_ActiveViewBuffer->PreviousMedia())
    {
        m_Renderer->Clear();
        ResetCacheMedia();
        m_CacheProcessor.RestartPlaybackCache();

        SetRange(m_ActiveViewBuffer->StartFrame(), m_ActiveViewBuffer->EndFrame());
    }
}

void Player::Connect()
{
    /* Timeline */
    connect(m_Timeline, &Timeline::TimeChanged, this, &Player::SetFrame);
    connect(m_Timeline, &Timeline::playbackStateChanged, this, [this](const Timeline::PlayState& state) -> void
    {
        if (state == Timeline::PlayState::STOPPED)
            m_CacheProcessor.StopPlaybackCache();
        else
            m_CacheProcessor.StartPlaybackCache(static_cast<ChronoFlux::Direction>(state));
    });
    connect(m_Timeline, &Timeline::mediaFinished, this, [this](const Timeline::PlayState& state) -> void
    {
        state == Timeline::PlayState::FORWARDS ? NextMedia() : PreviousMedia();
    });

    /* ControlBar */
    connect(m_ControlBar, &ControlBar::viewerBufferSwitched, this, &Player::ResetViewBuffer);
    connect(m_ControlBar, &ControlBar::comparisonModeChanged, this, &Player::SetComparisonMode);
    connect(m_ControlBar, &ControlBar::blendModeChanged, this, &Player::SetBlendMode);
}

void Player::ResetCacheMedia()
{
    switch (m_ActiveViewBuffer->PlayingComponent())
    {
        case ViewerBuffer::PlayableComponent::Sequence:
            m_CacheProcessor.SetSequence(m_ActiveViewBuffer->GetSequence());
            break;
        case ViewerBuffer::PlayableComponent::Track:
            m_CacheProcessor.SetTrack(m_ActiveViewBuffer->GetTrack());
            break;
        case ViewerBuffer::PlayableComponent::Clip:
        case ViewerBuffer::PlayableComponent::Playlist:
        default:
            m_CacheProcessor.SetMedia(m_ActiveViewBuffer->GetMediaClip());
    }
}

void Player::SetSequenceFrame(int frame)
{
    /* Ensure We have Valid sequence with media to process before setting the frame */
    if (m_ActiveViewBuffer->GetSequence()->IsEmpty())
        return;

    /* Try to get the trackItem at a given frame in the current sequence */
    /**
     * Instead of fetching the image data directly from the sequence, fetch the trackItem at any given point
     * This for the time being (till we have a best case O(1) for finding a track item at any given available)
     * is a better approach as the nearest frame logic causes a recursion which could end up looping over the same entity
     * Multiple times in order to find the media first, then it's nearest frame and then the image data for that frame which is very costly
     */
    SharedTrackItem item = m_ActiveViewBuffer->TrackItem(frame);

    if (item)
        SetTrackItemFrame(item, frame);
}

void Player::SetTrackFrame(int frame)
{
    /* Ensure We have Valid sequence with media to process before setting the frame */
    if (m_ActiveViewBuffer->GetTrack()->IsEmpty())
        return;

    /* Try to get the trackItem at a given frame in the current sequence */
    /**
     * Instead of fetching the image data directly from the sequence, fetch the trackItem at any given point
     * This for the time being (till we have a best case O(1) for finding a track item at any given available)
     * is a better approach as the nearest frame logic causes a recursion which could end up looping over the same entity
     * Multiple times in order to find the media first, then it's nearest frame and then the image data for that frame which is very costly
     */
    SharedTrackItem item = m_ActiveViewBuffer->TrackItem(frame);

    if (item)
        SetTrackItemFrame(item, frame);
}

void Player::SetTrackItemFrame(SharedTrackItem item, const int frame)
{
    /**
     * This GetImage itself runs a check if Media.Contains(frame) as it has to offset values internally
     * based on where the trackitem is present in the sequence
     */
    m_CacheProcessor.EnsureCached(frame);
    SharedPixels data = item->GetImage(frame);

    /* A standard frame which is available for any trackitem/media */
    if (data)
    {
        m_Renderer->Render(data);
    }
    else
    {
        /**
         * This maybe a case where the given frame does not exist for the track item
         * What happens next is based on how the missing frame handler is set
         */
        switch(m_MFrameHandler)
        {
            /* Show only black frame on the viewer */
            case MissingFrameHandler::BLACK_FRAME:
                m_Renderer->Clear();
                break;
            case MissingFrameHandler::ERROR_FRAME:
                m_Renderer->Clear();
                m_Renderer->SetMessage("Frame " + std::to_string(frame) + " not available.");
                break;
            case MissingFrameHandler::NEAREST:
                /**
                 * Recursively call the method but with the nearest available frame
                 * For any given frame, this recursion should happen only once as the nearest frame is a valid frame
                 * to read and render on the renderer
                 */
                SetTrackItemFrame(item, item->NearestFrame(frame));
                break;
        }
    }
}

void Player::SetMediaFrame(int frame)
{
    const SharedMediaClip& clip = m_ActiveViewBuffer->GetMediaClip();
    /* Ensure we have a valid media to process before setting the frame */
    if (clip->Empty())
        return;

    /**
     * If the frame does not have any data, this could mean that the frame is missing
     * if the provided frame is in range of the Media
     * How such a case is handled is based on the MissingFrameHandler
     * This determines what to do when a frame data is not available
     *
     * ERROR: Display an error on the Viewport stating the frame is not available.
     * BLACKFRAME: Display a black frame instead of anything else. No error is displayed.
     * NEAREST: Don't do anything here, as we continue to show the last frame which was rendered.
     */
    if (clip->Contains(frame))
    {
        m_CacheProcessor.EnsureCached(frame);
        /* Read the image for the frame from the sequence and set it on the player */
        m_Renderer->Render(clip->Image(frame), clip->Annotation(frame));
    }
    else
    {
        switch(m_MFrameHandler)
        {
            case MissingFrameHandler::BLACK_FRAME:
                m_Renderer->Clear();
                break;
            case MissingFrameHandler::ERROR_FRAME:
                m_Renderer->Clear();
                m_Renderer->SetMessage("Frame " + std::to_string(frame) + " not available.");
                break;
            case MissingFrameHandler::NEAREST:
                /**
                 * Recursively call the method but with the nearest available frame
                 * For any given frame, this recursion should happen only once as the nearest frame is a valid frame
                 * to read and render on the renderer
                 */
                SetMediaFrame(clip->NearestFrame(frame));
                break;
        }
    }
}

void Player::Compare(const SharedMediaClip& first, const SharedMediaClip& second)
{
    /* Update the Viewer Buffer with the Media Clips --> And then ask for them to be compared in the viewer */
    m_ViewBufferA->Set(first);
    m_ViewBufferB->Set(second);

    /* Compare frames */
    CompareMediaFrame(m_Timeline->Frame());
}

void Player::InspectCurrentMetadata()
{
    /**
     * Grab the active media from the ViewerBuffer that's active
     * if the viewer buffer has sequence set, then we grab the media that's at the very frame
     */
    if (Comparing())
        return;

    if (m_ActiveViewBuffer->PlayingComponent() == ViewerBuffer::PlayableComponent::Clip)
    {
        const SharedMediaClip& clip = m_ActiveViewBuffer->GetMediaClip();
        if (clip && clip->Valid())
            emit metadataInspected(clip);
    }
    else
    {
        const SharedTrackItem& item = m_ActiveViewBuffer->TrackItem(Frame());
        if (item)
            emit metadataInspected(item->GetMedia());
    }
}

void Player::CompareMediaFrame(v_frame_t frame)
{
    /* Compare on the Viewer */
    m_Renderer->Compare(m_ViewBufferA->Image(frame), m_ViewBufferB->Image(frame), m_ComparisonMode, m_BlendMode);
}

void Player::SetComparisonMode(int mode)
{
    /* Update Comparison Mode */
    m_ComparisonMode = static_cast<Renderer::ComparisonMode>(mode);

    if (m_ComparisonMode != Renderer::ComparisonMode::NONE)
    {
        /* Mark both the viewer Buffers as active */
        m_ViewBufferA->SetActive(true);
        m_ViewBufferB->SetActive(true);
    }
    else
    {
        /* Revert to the actual active Buffer */
        bool activeA = m_ActiveViewBuffer == m_ViewBufferA;
        /* Set its Active state */
        m_ViewBufferA->SetActive(activeA);
        /* Reset the active state for the other buffer */
        m_ViewBufferB->SetActive(!activeA);
    }

    /* Update to show the current frame */
    Refresh();
}

void Player::SetBlendMode(const int mode)
{
    /* Update the blend mode */
    m_BlendMode = static_cast<Renderer::BlendMode>(mode);

    if (m_ComparisonMode != Renderer::ComparisonMode::NONE)
    {
        /* Update to show the current frame */
        Refresh();
    }
}

void Player::ResetViewBuffer(const PlayerViewBuffer& buffer)
{
    /**
     * If we're currently comparing -> Then reset the Compare Mode to be None and select the buffer
     * which was currently opted
     */
    if (Comparing())
        m_ControlBar->SetCompareMode(Renderer::ComparisonMode::NONE);

    ViewerBuffer *active, *inactive;

    buffer == PlayerViewBuffer::A
        ? (active = m_ViewBufferA, inactive = m_ViewBufferB)
        : (active = m_ViewBufferB, inactive = m_ViewBufferA);

    m_ActiveViewBuffer = active;
    /* Update active states for the buffers */
    active->SetActive(true);
    inactive->SetActive(false);

    /* Clear the viewport */
    m_Renderer->Clear();

    /* Refresh the player with the updated content from the Buffer */
    ResetCacheMedia();
    Refresh();

    /* Update the frame range */
    SetRange(m_ActiveViewBuffer->StartFrame(), m_ActiveViewBuffer->EndFrame());
}


void Player::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat(MimeTypes::MediaItem) || event->mimeData()->hasFormat(MimeTypes::PlaylistItem))
    {
        /* Show Overlay */
        m_Overlay->setVisible(true);
        m_Overlay->setGeometry(m_Renderer->rect());

        event->acceptProposedAction();
    }
}

void Player::dragLeaveEvent(QDragLeaveEvent* event)
{
    m_Overlay->SetHoveredBuffer(PlayerOverlay::HoveredViewerBuffer::None);
    m_Overlay->setVisible(false);
}

void Player::dragMoveEvent(QDragMoveEvent* event)
{
    /* Update the Hovered Buffer based on the event Position */
    #if _QT6
    m_Overlay->SetHoveredBuffer(event->position().toPoint());
    #else
    m_Overlay->SetHoveredBuffer(event->pos());
    #endif // _QT6
}

void Player::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasFormat(MimeTypes::MediaItem))
    {
        QByteArray data = event->mimeData()->data(MimeTypes::MediaItem);

        /**
         * Media from the Media Bridge
         * The media is always retrieved from the active project
         * the assumption is that a drag-drop event would always happen when the project is active
         */
        const std::vector<SharedMediaClip>& media = _MediaBridge.UnpackProjectMedia(data);

        if (m_Overlay->HoveredBuffer() == PlayerOverlay::HoveredViewerBuffer::A)
        {
            media.size() == 1
                ? SetMedia(media.at(0), PlayerViewBuffer::A)
                : SetMedia(media, PlayerViewBuffer::A);
        }
        else if (m_Overlay->HoveredBuffer() == PlayerOverlay::HoveredViewerBuffer::B)
        {
            media.size() == 1
                ? SetMedia(media.at(0), PlayerViewBuffer::B)
                : SetMedia(media, PlayerViewBuffer::B);
        }
    }
    else if (event->mimeData()->hasFormat(MimeTypes::PlaylistItem))
    {
        QByteArray data = event->mimeData()->data(MimeTypes::PlaylistItem);

        /**
         * Media from the Media Bridge
         * The media is always retrieved from the active project
         * the assumption is that a drag-drop event would always happen when the project is active
         */
        const std::vector<SharedMediaClip>& media = _MediaBridge.UnpackPlaylistMedia(data);

        if (m_Overlay->HoveredBuffer() == PlayerOverlay::HoveredViewerBuffer::A)
        {
            media.size() == 1
                ? SetMedia(media.at(0), PlayerViewBuffer::A)
                : SetMedia(media, PlayerViewBuffer::A);
        }
        else if (m_Overlay->HoveredBuffer() == PlayerOverlay::HoveredViewerBuffer::B)
        {
            media.size() == 1
                ? SetMedia(media.at(0), PlayerViewBuffer::B)
                : SetMedia(media, PlayerViewBuffer::B);
        }
    }

    /* Reset Overlay */
    m_Overlay->SetHoveredBuffer(PlayerOverlay::HoveredViewerBuffer::None);
    m_Overlay->setVisible(false);
}

VOID_NAMESPACE_CLOSE
