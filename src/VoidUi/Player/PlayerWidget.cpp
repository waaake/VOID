// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QKeyEvent>
#include <QMimeData>
#include <QPaintEvent>
#include <QPainter>

/* Internal */
#include "PlayerWidget.h"
#include "VoidUi/Descriptors.h"
#include "VoidUi/Preferences/Preferences.h"
#include "VoidUi/Media/MediaBridge.h"

VOID_NAMESPACE_OPEN

Player::Player(QWidget* parent)
    : QWidget(parent)
    , m_ComparisonMode(Renderer::ComparisonMode::NONE)
    , m_BlendMode(Renderer::BlendMode::UNDER)
    , m_MFrameHandler(static_cast<MissingFrameHandler>(VoidPreferences::Instance().GetMissingFrameHandler()))
{
    /* Init the Viewer Buffer */
    m_ViewBufferA = new ViewerBuffer("A");
    m_ViewBufferB = new ViewerBuffer("B");

    /* Set the Associated Color for Viewer Buffer B */
    m_ViewBufferB->SetColor(QColor(70, 180, 220));      // Blue

    /* The default buffer is Viewer Buffer A */
    m_ActiveViewBuffer = m_ViewBufferA;
    m_ViewBufferA->SetActive(true);

    m_CacheProcessor.SetActivePlayer(this);

    /* Setup the OpenGL Profile before the OpenGL Context is initialised */
    VoidRenderer::SetProfile();

    /* Build the layout */
    Build();

    /* Connect Signals */
    Connect();

    /* Accept drops */
    setAcceptDrops(true);
}

Player::~Player()
{
    /* Cleanup the buffers */
    if (m_ViewBufferA)
    {
        delete m_ViewBufferA;
        m_ViewBufferA = nullptr;
    }
    if (m_ViewBufferB)
    {
        delete m_ViewBufferB;
        m_ViewBufferB = nullptr;
    }

    m_ActiveViewBuffer = nullptr;
}

void Player::Connect()
{
    /* Timeline - TimeChange -> Player - SetFrame */
    connect(m_Timeline, &Timeline::TimeChanged, this, &Player::SetFrame);
    /* Timeline - fullscreenRequested -> Player - SetRendererFullscreen */
    connect(m_Timeline, &Timeline::fullscreenRequested, this, &Player::SetRendererFullscreen);
    connect(m_Timeline, &Timeline::playbackStateChanged, this, [this](const Timeline::PlayState& state)
    {
        if (state == Timeline::PlayState::STOPPED)
            m_CacheProcessor.StopPlaybackCache();
        else
            m_CacheProcessor.StartPlaybackCache(static_cast<ChronoFlux::Direction>(state));
    });

    /* ControlBar - ZoomChange -> Renderer - UpdateZoom */
    connect(m_ControlBar, &ControlBar::zoomChanged, m_Renderer, &VoidRenderer::UpdateZoom);
    /* ControlBar - ExposureChange -> Renderer - SetExposure */
    connect(m_ControlBar, &ControlBar::exposureChanged, m_Renderer, &VoidRenderer::SetExposure);
    /* ControlBar - GammaChange -> Renderer - SetGamma */
    connect(m_ControlBar, &ControlBar::gammaChanged, m_Renderer, &VoidRenderer::SetGamma);
    /* ControlBar - GainChange -> Renderer - SetGain */
    connect(m_ControlBar, &ControlBar::gainChanged, m_Renderer, &VoidRenderer::SetGain);
    /* ControlBar - ChannelModeChanged -> Renderer - SetChannelMode */
    connect(m_ControlBar, &ControlBar::channelModeChanged, m_Renderer, &VoidRenderer::SetChannelMode);
    /* ControlBar - Viewer Buffer Switched -> Player - Set View Buffer */
    connect(m_ControlBar, &ControlBar::viewerBufferSwitched, this, &Player::SetViewBuffer);
    /* ControlBar - Comparison Mode Changed -> Player - Set Comparison mode */
    connect(m_ControlBar, &ControlBar::comparisonModeChanged, this, &Player::SetComparisonMode);
    /* ControlBar - Blend Mode Changed -> Player - Set Blend mode */
    connect(m_ControlBar, &ControlBar::blendModeChanged, this, &Player::SetBlendMode);
    /* ControlBar - Annotations Toggled -> AnnotationsController - Set Visible */
    connect(m_ControlBar, &ControlBar::annotationsToggled, this, &Player::ToggleAnnotations);
    /* ControlBar - Color Display Changed -> Renderer - Set Color Display */
    connect(m_ControlBar, &ControlBar::colorDisplayChanged, m_Renderer, &VoidRenderer::SetColorDisplay);

    /* Annotations Controller - Cleared -> Renderer - Clear Annotations */
    connect(m_AnnotationsController, &AnnotationsController::cleared, m_Renderer, &VoidRenderer::ClearAnnotations);
    /* Annotations Controller - Color Changed -> Renderer - Set Annotation Color */
    connect(m_AnnotationsController, &AnnotationsController::colorChanged, m_Renderer, static_cast<void(VoidRenderer::*)(const QColor&)>(&VoidRenderer::SetAnnotationColor));
    /* Annotations Controller - Brush Size Changed -> Renderer - Set Annotation Size */
    connect(m_AnnotationsController, &AnnotationsController::brushSizeChanged, m_Renderer, &VoidRenderer::SetAnnotationSize);
    /* Annotations Controller - Control Changed -> Renderer - Set Annotation DrawType */
    connect(m_AnnotationsController, &AnnotationsController::controlChanged, m_Renderer, &VoidRenderer::SetAnnotationDrawType);

    /* Preference - updated -> Player - SetFromPreferences */
    connect(&VoidPreferences::Instance(), &VoidPreferences::updated, this, &Player::SetFromPreferences);

    /* Renderer - exitFullscreen -> Player - Exitfullscreen */
    connect(m_Renderer, &VoidRenderer::exitFullscreen, this, &Player::ExitFullscreenRenderer);
    /* Connect Play Controls from Renderer */
    connect(m_Renderer, &VoidRenderer::playForwards, this, &Player::PlayForwards);
    connect(m_Renderer, &VoidRenderer::playBackwards, this, &Player::PlayBackwards);
    connect(m_Renderer, &VoidRenderer::stop, this, &Player::Stop);
    connect(m_Renderer, &VoidRenderer::moveForward, this, &Player::NextFrame);
    connect(m_Renderer, &VoidRenderer::moveBackward, this, &Player::PreviousFrame);

    /* Renderer - Annotation Created -> ViewerBuffer - SetAnnotation */
    connect(m_Renderer, &VoidRenderer::annotationCreated, this, &Player::AddAnnotation);
    /* Renderer - Annotation Deleted -> ViewerBuffer - RemoveAnnotation */
    connect(m_Renderer, &VoidRenderer::annotationDeleted, this, &Player::RemoveAnnotation);

    /* When a MediaClip is about to be removed from the MediaBride */
    connect(&_MediaBridge, &MBridge::mediaAboutToBeRemoved, this, &Player::RemoveMedia);
}

void Player::RemoveMedia(const SharedMediaClip& media)
{
    /* Check if the media was playing currently */
    if (m_ActiveViewBuffer->Playing(media))
    {
        /* Clear the active Viewer Buffer */
        m_ActiveViewBuffer->Clear();
        /* Clear the player */
        Clear();
    }
}

void Player::SetFromPreferences()
{
    VOID_LOG_INFO("Player Preferences Updated.");
    /* Reset the Missing Frame Hanlder */
    SetMissingFrameHandler(VoidPreferences::Instance().GetMissingFrameHandler());
}

void Player::CacheBuffer()
{
    /* Cache the Buffer which is active (or both if in comparison mode) */
    if (m_ViewBufferA->Active())
        m_ViewBufferA->Cache();
    if (m_ViewBufferB->Active())
        m_ViewBufferB->Cache();
}

void Player::Build()
{
    /* Base layout for the widget */
    QVBoxLayout* layout = new QVBoxLayout(this);

    QHBoxLayout* horizontalInternal = new QHBoxLayout;
    horizontalInternal->setContentsMargins(0, 0, 0, 0);

    m_AnnotationsController = new AnnotationsController(this);
    /* Is Hidden by default and controlled by Annotations Toggle Controller */
    m_AnnotationsController->setVisible(false);

    /* Renderer's house */
    m_RendererLayout = new QVBoxLayout;
    m_RendererLayout->setContentsMargins(0, 0, 0, 0);

    /* Instantiate widgets */
    m_ControlBar = new ControlBar(m_ViewBufferA, m_ViewBufferB, this);
    m_Renderer = new VoidRenderer(this);
    /* The placeholder renderer for when the actual renderer is fullscreen */
    m_PlaceholderRenderer = new VoidPlaceholderRenderer(this);
    /* Is hidden by default */
    m_PlaceholderRenderer->setVisible(false);
    m_Timeline = new Timeline(this);

    m_Overlay = new PlayerOverlay(m_Renderer);
    m_Overlay->setVisible(false);

    /**
     * The way how this renderer will be setup in UI is
     * First Row will have any controls related to the viewport and anything which can show any information
     * The second Row/section will be the one which holds the Renderer
     * Then comes the Timeslider which holds any controls for playback
     */
    layout->addWidget(m_ControlBar);
    /* Add to the Renderer Layout */
    m_RendererLayout->addWidget(m_Renderer);
    m_RendererLayout->addWidget(m_PlaceholderRenderer);

    horizontalInternal->addLayout(m_RendererLayout);
    horizontalInternal->addWidget(m_AnnotationsController);

    layout->addLayout(horizontalInternal);

    layout->addWidget(m_Timeline);

    /* Spacing */
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 2);
}

void Player::Clear()
{
    /**
     * Update the time range to be 0-1 ??
     * Clear the data from the player
     */
    m_Timeline->SetRange(0, 1);
    m_Timeline->Clear();
    m_Renderer->Clear();
}

void Player::Load(const SharedMediaClip& media)
{
    /* Update what's currently being played on the viewer buffer */
    m_ActiveViewBuffer->Set(media);
    m_CacheProcessor.SetMedia(media);

    /* Viewer Buffer - Clip -> Player - Add Cache Frame */
    ConnectMediaClipToTimeline(media);

    /**
     * Once we have the image sequence,
     * First update the timeslider range,
     */
    m_Timeline->SetRange(media->FirstFrame(), media->LastFrame());
    /* Clear any cached frame markings from the timeline */
    m_Timeline->Clear();
    m_Timeline->SetAnnotatedFrames(std::move(media->AnnotatedFrames()));

    SetMediaFrame(m_Timeline->Frame());
}

void Player::Load(const SharedMediaClip& media, const PlayerViewBuffer& buffer)
{
    if (buffer == PlayerViewBuffer::A)
        m_ViewBufferA->Set(media);
    else if (buffer == PlayerViewBuffer::B)
        m_ViewBufferB->Set(media);

    /* Update or Reset Buffer */
    if (Comparing())
        Refresh();
    else
        SetViewBuffer(buffer);
}

void Player::Load(const SharedPlaybackTrack& track)
{
    /* Update what's currently being played on the viewer buffer */
    m_ActiveViewBuffer->Set(track);
    m_CacheProcessor.SetTrack(track);

    /**
     * Once we have the image sequence,
     * First update the timeslider range,
     */
    m_Timeline->SetRange(track->StartFrame(), track->EndFrame());
    /* Clear any cached frame markings from the timeline */
    m_Timeline->Clear();
    /* Then set the First Image on the Player */
    SetTrackFrame(m_Timeline->Frame());
}

void Player::Load(const SharedPlaybackSequence& sequence)
{
    /* Update what is being played on the Active Viewer Buffer */
    m_ActiveViewBuffer->Set(sequence);
    m_CacheProcessor.SetSequence(sequence);

    /**
     * Once we have the image sequence,
     * First update the timeslider range,
     */
    m_Timeline->SetRange(sequence->StartFrame(), sequence->EndFrame());
    /* Clear any cached frame markings from the timeline */
    m_Timeline->Clear();

    /* Render the current frame from the seqeunce */
    SetSequenceFrame(m_Timeline->Frame());

    VOID_LOG_INFO("SEQUENCE SET: Range: {0}--{1}", sequence->StartFrame(), sequence->EndFrame());
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
     * BLACK_FRAME: Display a black frame instead of anything else. No error is displayed.
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

void Player::SetViewBuffer(const PlayerViewBuffer& buffer)
{
    /**
     * If we're currently comparing -> Then reset the Compare Mode to be None and select the buffer
     * which was currently opted
     */
    if (Comparing())
        m_ControlBar->SetCompareMode(Renderer::ComparisonMode::NONE);

    /* Update the active buffer based on the provided buffer enum */
    if (buffer == PlayerViewBuffer::A)
    {
        m_ActiveViewBuffer = m_ViewBufferA;
        /* Set its Active state */
        m_ViewBufferA->SetActive(true);
        /* Reset the active state for the other buffer */
        m_ViewBufferB->SetActive(false);
    }
    else
    {
        m_ActiveViewBuffer = m_ViewBufferB;
        /* Set its Active state */
        m_ViewBufferB->SetActive(true);
        /* Reset the active state for the other buffer */
        m_ViewBufferA->SetActive(false);
    }

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

        /* Read Input data */
        QDataStream stream(&data, QIODevice::ReadOnly);
        int row, column;
        stream >> row >> column;

        /**
         * Media from the Media Bridge
         * The media is always retrieved from the active project
         * the assumption is that a drag-drop event would always happen when the project is active
         */
        SharedMediaClip media = _MediaBridge.MediaAt(row, column);

        if (m_Overlay->HoveredBuffer() == PlayerOverlay::HoveredViewerBuffer::A)
            Load(media, PlayerViewBuffer::A);
        else if (m_Overlay->HoveredBuffer() == PlayerOverlay::HoveredViewerBuffer::B)
            Load(media, PlayerViewBuffer::B);
    }
    else if (event->mimeData()->hasFormat(MimeTypes::PlaylistItem))
    {
        QByteArray data = event->mimeData()->data(MimeTypes::PlaylistItem);

        /* Read Input data */
        QDataStream stream(&data, QIODevice::ReadOnly);
        int row, column;
        stream >> row >> column;

        /**
         * Media from the Media Bridge
         * The media is always retrieved from the active project
         * the assumption is that a drag-drop event would always happen when the project is active
         */
        SharedMediaClip media = _MediaBridge.PlaylistMediaAt(row, column);

        if (m_Overlay->HoveredBuffer() == PlayerOverlay::HoveredViewerBuffer::A)
            Load(media, PlayerViewBuffer::A);
        else if (m_Overlay->HoveredBuffer() == PlayerOverlay::HoveredViewerBuffer::B)
            Load(media, PlayerViewBuffer::B);
    }

    /* Reset Overlay */
    m_Overlay->SetHoveredBuffer(PlayerOverlay::HoveredViewerBuffer::None);
    m_Overlay->setVisible(false);
}

void Player::SetRendererFullscreen()
{
    /* Set Renderer as Fullscreen */
    m_Renderer->PrepareFullscreen();
    m_Renderer->showFullScreen();

    /* Show the Placeholder Renderer in place */
    m_PlaceholderRenderer->setVisible(true);
}

void Player::ExitFullscreenRenderer()
{
    if (!m_Renderer->Fullscreen())
        return;

    /* Reset the parent of the Renderer back to this widget */
    m_Renderer->setParent(this);

    /* Add the renderer back */
    m_RendererLayout->addWidget(m_Renderer);

    /* Hide the Placeholder Renderer */
    m_PlaceholderRenderer->setVisible(false);

    /* We're back normal screened */
    m_Renderer->ExitFullscreen();
}

void Player::ToggleAnnotations(const bool state)
{
    /* Show/Hide the annotation controller based on the state */
    m_AnnotationsController->setVisible(state);

    /* Setup Annotations on the Renderer */
    m_Renderer->ToggleAnnotation(state);

    /**
     * Also Make the Render view as default to allow annotating correctly
     * TODO: At a later stage we can enable panning with Annotations
     * allowing to pan with ALT + Click Drag
     */
    m_Renderer->ZoomToFit();

    /* If we're in compare mode -> Reset that as we only want to annotate a frame in the buffer */
    if (Comparing())
        m_ControlBar->SetCompareMode(Renderer::ComparisonMode::NONE);
}

void Player::AddAnnotation(const Renderer::SharedAnnotation& annotation)
{
    /* Save the Annotation */
    m_ActiveViewBuffer->SetAnnotation(m_Timeline->Frame(), annotation);
    /* Also Mark that the frame has been annotated */
    m_Timeline->AddAnnotatedFrame(m_Timeline->Frame());
}

void Player::RemoveAnnotation()
{
    /* Remove Annotation from the underlying Media */
    m_ActiveViewBuffer->RemoveAnnotation(m_Timeline->Frame());
    /* Remove the annotated frame from the Timeline */
    m_Timeline->RemoveAnnotatedFrame(m_Timeline->Frame());
}

VOID_NAMESPACE_CLOSE
