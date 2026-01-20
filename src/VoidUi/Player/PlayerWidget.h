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
#include "VoidRenderer/RenderTypes.h"
#include "VoidRenderer/VoidRenderer.h"
#include "VoidUi/Timeline/Timeline.h"
#include "VoidUi/Toolkit/AnnotationController.h"
#include "VoidUi/Toolkit/ControlBar.h"
#include "VoidUi/Media/MediaCache.h"

VOID_NAMESPACE_OPEN

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
    PlayerWidget(QWidget* parent = nullptr);
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
    [[nodiscard]] inline bool Fullscreen() { return m_Renderer->Fullscreen(); }
    /**
     * Returns true if the current comparison mode is not Compare None
     */
    [[nodiscard]] inline bool Comparing() { return m_ComparisonMode != Renderer::ComparisonMode::NONE; }

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

    /* Zoom on the Viewport */
    inline void ZoomIn() { m_Renderer->ZoomIn(); }
    inline void ZoomOut() { m_Renderer->ZoomOut(); }
    inline void ZoomToFit() { m_Renderer->ZoomToFit(); }

    /* Mark a frame on the timeline as cached */
    inline void AddCacheFrame(int frame) { m_Timeline->AddCacheFrame(frame); }
    inline void RemoveCachedFrame(int frame) { m_Timeline->RemoveCachedFrame(frame); }
    inline void ClearCachedFrames() { m_Timeline->ClearCachedFrames(); }

    /* Set Range on the timeline */
    inline void SetRange(int start, int end) { m_Timeline->SetRange(start, end); }

    /**
     * Removes the MediaClip from the player buffer, if found
     */
    void RemoveMedia(const SharedMediaClip& media);

    /**
     * Timeline Controls:
     * Below methods expose the functionality from the timeline
     * These are required to be able to govern the play state or set any frame according
     * to the current timeline state
     * These would get invoked from the Menu or via shortcuts
     * Eventually making their way onto the CPython exposed API for void python API
     */
    inline void PlayForwards() { m_Timeline->PlayForwards(); }
    inline void PlayBackwards() { m_Timeline->PlayBackwards(); }

    inline void Stop() { m_Timeline->Stop(); }

    inline v_frame_t Frame() const { return m_Timeline->Frame(); }
    inline void NextFrame() { m_Timeline->NextFrame(); }
    inline void PreviousFrame() { m_Timeline->PreviousFrame(); }

    inline void MoveToStart() { m_Timeline->MoveToStart(); }
    inline void MoveToEnd() { m_Timeline->MoveToEnd(); }

    inline void SetUserFirstframe(int frame) { m_Timeline->SetUserFirstframe(frame); }
    inline void SetUserEndframe(int frame) { m_Timeline->SetUserEndframe(frame); }

    /**
     * (Re)sets the In and out framing of the Timeslider
     * Calling it once sets the frame as in/out frame (User-In/User-Out)
     * Calling it the next time on the same frame Resets the in/out frame (User-In/User-Out)
     */
    inline void ResetInFrame() { m_Timeline->ResetInFrame(); }
    inline void ResetOutFrame() { m_Timeline->ResetOutFrame(); }
    inline void ResetRange() { m_Timeline->ResetRange(); }
    inline void ReserveRange(int duration) { m_Timeline->ReserveRange(duration); }

public:
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
    Timeline* m_Timeline;

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
