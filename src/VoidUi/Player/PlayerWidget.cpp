// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPaintEvent>
#include <QPainter>

/* Internal */
#include "PlayerWidget.h"
#include "VoidCore/Logging.h"
#include "VoidObjects/Preferences/Preferences.h"
#include "VoidUi/Media/MediaBridge.h"

VOID_NAMESPACE_OPEN

PlayerWidget::PlayerWidget(TimelineController* timelineController, QWidget* parent)
    : QWidget(parent)
    , m_TimelineController(timelineController)
    , m_ComparisonMode(Renderer::ComparisonMode::NONE)
    , m_BlendMode(Renderer::BlendMode::UNDER)
    , m_MFrameHandler(static_cast<MissingFrameHandler>(VoidPreferences::Instance().GetMissingFrameHandler()))
{
    m_ViewBufferA = new ViewerBuffer(timelineController);
    m_ViewBufferB = new ViewerBuffer(timelineController);

    m_ViewBufferA->SetName("A");
    m_ViewBufferB->SetName("B");

    m_ViewBufferB->SetColor(QColor(70, 180, 220));      // Blue

    // The default buffer is Viewer Buffer A
    m_ActiveViewBuffer = m_ViewBufferA;
    m_ViewBufferA->SetActive(true);

    // Needs to be always before we initialize Renderer
    VoidRenderer::SetProfile();

    Build();
    Connect();
    setAcceptDrops(true);
}

PlayerWidget::~PlayerWidget()
{
    m_ActiveViewBuffer = nullptr;

    m_ViewBufferA->deleteLater();
    delete m_ViewBufferA;
    m_ViewBufferA = nullptr;

    m_ViewBufferB->deleteLater();
    delete m_ViewBufferB;
    m_ViewBufferB = nullptr;
}

void PlayerWidget::Connect()
{
    // TimelineController
    connect(m_TimelineController, &TimelineController::fullscreenRequested, this, &PlayerWidget::SetRendererFullscreen);

    // ControlBar
    connect(m_ControlBar, &ControlBar::zoomChanged, m_Renderer, &VoidRenderer::SetZoom);
    connect(m_ControlBar, &ControlBar::exposureChanged, m_Renderer, &VoidRenderer::SetExposure);
    connect(m_ControlBar, &ControlBar::gammaChanged, m_Renderer, &VoidRenderer::SetGamma);
    connect(m_ControlBar, &ControlBar::gainChanged, m_Renderer, &VoidRenderer::SetGain);
    connect(m_ControlBar, &ControlBar::channelModeChanged, m_Renderer, &VoidRenderer::SetChannelMode);
    connect(m_ControlBar, &ControlBar::annotationsToggled, this, &PlayerWidget::ToggleAnnotations);
    connect(m_ControlBar, &ControlBar::colorDisplayChanged, m_Renderer, &VoidRenderer::SetColorDisplay);

    // AnnotationsController
    connect(m_AnnotationsController, &AnnotationsController::cleared, m_Renderer, &VoidRenderer::ClearAnnotations);
    connect(m_AnnotationsController, &AnnotationsController::colorChanged, m_Renderer, static_cast<void (VoidRenderer::*)(const QColor&)>(&VoidRenderer::SetAnnotationColor));
    connect(m_AnnotationsController, &AnnotationsController::brushSizeChanged, m_Renderer, &VoidRenderer::SetAnnotationSize);
    connect(m_AnnotationsController, &AnnotationsController::controlChanged, m_Renderer, &VoidRenderer::SetAnnotationDrawType);

    // VoidPreferences
    connect(&VoidPreferences::Instance(), &VoidPreferences::updated, this, &PlayerWidget::SetFromPreferences);

    // Renderer
    connect(m_Renderer, &VoidRenderer::exitFullscreen, this, &PlayerWidget::ExitFullscreenRenderer);
    connect(m_Renderer, &VoidRenderer::playForwards, m_TimelineController, &TimelineController::PlayForwards);
    connect(m_Renderer, &VoidRenderer::playBackwards, m_TimelineController, &TimelineController::PlayBackwards);
    connect(m_Renderer, &VoidRenderer::stop, m_TimelineController, &TimelineController::Stop);
    connect(m_Renderer, &VoidRenderer::moveForward, m_TimelineController, &TimelineController::NextFrame);
    connect(m_Renderer, &VoidRenderer::moveBackward, m_TimelineController, &TimelineController::PreviousFrame);
    connect(m_Renderer, &VoidRenderer::annotationCreated, this, &PlayerWidget::AddAnnotation);
    connect(m_Renderer, &VoidRenderer::annotationDeleted, this, &PlayerWidget::RemoveAnnotation);
    connect(m_Renderer, &VoidRenderer::zoomChanged, m_ControlBar, &ControlBar::SetZoom);

    // Bridge
    connect(&_MediaBridge, &MBridge::mediaAboutToBeRemoved, this, &PlayerWidget::RemoveMedia, Qt::DirectConnection);
}

void PlayerWidget::RemoveMedia(const SharedMediaClip& media)
{
    // Check if the media was playing currently
    if (m_ActiveViewBuffer->Playing(media))
    {
        m_ActiveViewBuffer->Clear();
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
    layout->addWidget(m_TimelineController->TimelineWidget());

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
    m_TimelineController->Clear();
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
    m_RendererLayout->addWidget(m_Renderer);
    m_PlaceholderRenderer->setVisible(false);
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
    m_ActiveViewBuffer->SetAnnotation(m_TimelineController->Frame(), annotation);
    m_TimelineController->MarkAnnotated(m_TimelineController->Frame());
}

void PlayerWidget::RemoveAnnotation()
{
    m_ActiveViewBuffer->RemoveAnnotation(m_TimelineController->Frame());
    m_TimelineController->RemoveAnnotated(m_TimelineController->Frame());
}

VOID_NAMESPACE_CLOSE
