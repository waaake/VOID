// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPaintEvent>
#include <QPainter>

/* Internal */
#include "PlayerWidget.h"
#include "VoidCore/Logging.h"
#include "VoidUi/Preferences/Preferences.h"
#include "VoidUi/Media/MediaBridge.h"

VOID_NAMESPACE_OPEN

PlayerWidget::PlayerWidget(QWidget* parent)
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

    /* Setup the OpenGL Profile before the OpenGL Context is initialised */
    VoidRenderer::SetProfile();

    /* Build the layout */
    Build();

    /* Connect Signals */
    Connect();

    /* Accept drops */
    setAcceptDrops(true);
}

PlayerWidget::~PlayerWidget()
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

void PlayerWidget::Connect()
{
    /* Timeline - fullscreenRequested -> Player - SetRendererFullscreen */
    connect(m_Timeline, &Timeline::fullscreenRequested, this, &PlayerWidget::SetRendererFullscreen);

    /* ControlBar - ZoomChange -> Renderer - SetZoom */
    connect(m_ControlBar, &ControlBar::zoomChanged, m_Renderer, &VoidRenderer::SetZoom);
    /* ControlBar - ExposureChange -> Renderer - SetExposure */
    connect(m_ControlBar, &ControlBar::exposureChanged, m_Renderer, &VoidRenderer::SetExposure);
    /* ControlBar - GammaChange -> Renderer - SetGamma */
    connect(m_ControlBar, &ControlBar::gammaChanged, m_Renderer, &VoidRenderer::SetGamma);
    /* ControlBar - GainChange -> Renderer - SetGain */
    connect(m_ControlBar, &ControlBar::gainChanged, m_Renderer, &VoidRenderer::SetGain);
    /* ControlBar - ChannelModeChanged -> Renderer - SetChannelMode */
    connect(m_ControlBar, &ControlBar::channelModeChanged, m_Renderer, &VoidRenderer::SetChannelMode);
    /* ControlBar - Annotations Toggled -> AnnotationsController - Set Visible */
    connect(m_ControlBar, &ControlBar::annotationsToggled, this, &PlayerWidget::ToggleAnnotations);
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
    connect(&VoidPreferences::Instance(), &VoidPreferences::updated, this, &PlayerWidget::SetFromPreferences);

    /* Renderer - exitFullscreen -> Player - Exitfullscreen */
    connect(m_Renderer, &VoidRenderer::exitFullscreen, this, &PlayerWidget::ExitFullscreenRenderer);
    /* Connect Play Controls from Renderer */
    connect(m_Renderer, &VoidRenderer::playForwards, this, &PlayerWidget::PlayForwards);
    connect(m_Renderer, &VoidRenderer::playBackwards, this, &PlayerWidget::PlayBackwards);
    connect(m_Renderer, &VoidRenderer::stop, this, &PlayerWidget::Stop);
    connect(m_Renderer, &VoidRenderer::moveForward, this, &PlayerWidget::NextFrame);
    connect(m_Renderer, &VoidRenderer::moveBackward, this, &PlayerWidget::PreviousFrame);

    /* Renderer - Annotation Created -> ViewerBuffer - SetAnnotation */
    connect(m_Renderer, &VoidRenderer::annotationCreated, this, &PlayerWidget::AddAnnotation);
    /* Renderer - Annotation Deleted -> ViewerBuffer - RemoveAnnotation */
    connect(m_Renderer, &VoidRenderer::annotationDeleted, this, &PlayerWidget::RemoveAnnotation);
    connect(m_Renderer, &VoidRenderer::zoomChanged, m_ControlBar, &ControlBar::SetZoom);

    /* When a MediaClip is about to be removed from the MediaBride */
    connect(&_MediaBridge, &MBridge::mediaAboutToBeRemoved, this, &PlayerWidget::RemoveMedia);
}

void PlayerWidget::RemoveMedia(const SharedMediaClip& media)
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

void PlayerWidget::SetFromPreferences()
{
    VOID_LOG_INFO("Player Preferences Updated.");
    /* Reset the Missing Frame Hanlder */
    SetMissingFrameHandler(VoidPreferences::Instance().GetMissingFrameHandler());
}

void PlayerWidget::Build()
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

void PlayerWidget::Clear()
{
    /**
     * Update the time range to be 0-1 ??
     * Clear the data from the player
     */
    m_Timeline->SetRange(0, 1);
    m_Timeline->Clear();
    m_Renderer->Clear();
}

void PlayerWidget::SetRendererFullscreen()
{
    /* Set Renderer as Fullscreen */
    m_Renderer->PrepareFullscreen();
    m_Renderer->showFullScreen();

    /* Show the Placeholder Renderer in place */
    m_PlaceholderRenderer->setVisible(true);
}

void PlayerWidget::ExitFullscreenRenderer()
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

void PlayerWidget::ToggleAnnotations(const bool state)
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

void PlayerWidget::AddAnnotation(const Renderer::SharedAnnotation& annotation)
{
    /* Save the Annotation */
    m_ActiveViewBuffer->SetAnnotation(m_Timeline->Frame(), annotation);
    /* Also Mark that the frame has been annotated */
    m_Timeline->AddAnnotatedFrame(m_Timeline->Frame());
}

void PlayerWidget::RemoveAnnotation()
{
    /* Remove Annotation from the underlying Media */
    m_ActiveViewBuffer->RemoveAnnotation(m_Timeline->Frame());
    /* Remove the annotated frame from the Timeline */
    m_Timeline->RemoveAnnotatedFrame(m_Timeline->Frame());
}

VOID_NAMESPACE_CLOSE
