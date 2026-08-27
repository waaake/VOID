// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_PLAYER_WIDGET_H
#define _VOID_PLAYER_WIDGET_H

/* Qt */
#include <QLayout>
#include <QWidget>

/* Internal */
#include "QDefinition.h"
#include "ViewerBuffer.h"
#include "OverlayWidget.h"
#include "VoidObjects/Media/MediaClip.h"
#include "VoidObjects/Sequence/Sequence.h"
#include "VoidTimeline/TimelineController.h"
#include "VoidMediaPlayer/Toolkit/AnnotationController.h"
#include "VoidMediaPlayer/Toolkit/ControlBar.h"

VOID_NAMESPACE_OPEN

class VoidRenderer;
class VoidPlaceholderRenderer;

class VOID_API PlayerWidget : public QWidget
{
    Q_OBJECT

public:
    /* Reprents how to handle missing frame */
    enum class MissingFrameHandler : short
    {
        /* An error is displayed on the Viewport */
        ERROR_FRAME,
        /* Nothing is displayed on the viewport */
        BLACK_FRAME,
        /* Continue to display the last frame on the viewport */
        NEAREST
    };

public:
    PlayerWidget(TimelineController* timelineController, QWidget* parent = nullptr);
    virtual ~PlayerWidget();

    /* Getters */
    inline SharedMediaClip ActiveMediaClip() const
    { 
        /**
         * Check if the current playing component is a clip, if so -> return the clip from the ViewerBuffer
         */
        if (m_ActiveViewBuffer->PlayingComponent() == ViewerBuffer::PlayableComponent::Clip)
            return m_ActiveViewBuffer->GetMediaClip();

        /* Not Currently playing a clip -> return nullptr instead */
        return nullptr;
    }

    inline SharedPlaybackTrack GetTrack() const { return m_ActiveViewBuffer->GetTrack(); }

    /* Fullscreen Renderer */
    void SetRendererFullscreen();
    void ExitFullscreenRenderer();
    /**
     * Returns whether the player is currently fullscreen or not
     */
    // [[nodiscard]] inline bool Fullscreen() { return m_Renderer->Fullscreen(); }
    /**
     * Returns true if the current comparison mode is not Compare None
     */
    [[nodiscard]] inline bool Comparing() { return m_ComparisonMode != Renderer::ComparisonMode::NONE; }
    [[nodiscard]] inline bool ActiveGrid() { return m_ComparisonMode == Renderer::ComparisonMode::GRID; }

    /**
     * Updates the Handler for Missing frame
     * Setting the behaviour for when a missing frame is set
     */
    inline void SetMissingFrameHandler(int handler)
    {
        /* Update the missing frame handler */
        m_MFrameHandler = static_cast<MissingFrameHandler>(handler);
        /* And Refresh the viewport */
        // Refresh();
    }

    // Zoom on the Viewport
    void ZoomIn();
    void ZoomOut();
    void ZoomToFit();

    int GridRows() const;
    int GridColumns() const;
    void SetGridRows(int rows);
    void SetGridColumns(int columns);

    /**
     * Removes the MediaClip from the player buffer, if found
     */
    void RemoveMedia(const SharedMediaClip& media);

    void Clear();

protected:  /* Methods */
    void Build();
    void Connect();
    
    void ToggleAnnotations(const bool state);
    void AddAnnotation(const Renderer::SharedAnnotation& annotation);
    void RemoveAnnotation();

    void SetFromPreferences();
    
protected:  /* Members */
    QVBoxLayout* m_RendererLayout;

    PlayerOverlay* m_Overlay;
    VoidRenderer* m_Renderer;
    VoidPlaceholderRenderer* m_PlaceholderRenderer;
    TimelineController* m_TimelineController;

    ControlBar* m_ControlBar;
    AnnotationsController* m_AnnotationsController;

    Renderer::ComparisonMode m_ComparisonMode;
    Renderer::BlendMode m_BlendMode;

    ViewerBuffer* m_ViewBufferA;
    ViewerBuffer* m_ViewBufferB;
    ViewerBuffer* m_ActiveViewBuffer;

    MissingFrameHandler m_MFrameHandler;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PLAYER_WIDGET_H
