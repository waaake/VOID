#ifndef _VOID_PLAYER_WIDGET_H
#define _VOID_PLAYER_WIDGET_H

/* Qt */
#include <QWidget>
#include <QLayout>

/* Internal */
#include "AnnotationController.h"
#include "ControlBar.h"
#include "Definition.h"
#include "MediaClip.h"
#include "Sequence.h"
#include "ViewerBuffer.h"
#include "VoidCore/Media.h"
#include "Timeline/Timeline.h"
#include "VoidRenderer/RenderTypes.h"
#include "VoidRenderer/Renderer.h"

VOID_NAMESPACE_OPEN

class Player : public QWidget
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
    Player(QWidget* parent = nullptr);
    virtual ~Player();

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

    /* Loads a Playable Media (clip) on the Player */
    void Load(const SharedMediaClip& media);

    /* Loads a Playable Track on the Player */
    void Load(const SharedPlaybackTrack& track);

    /* Load a Sequence to be played on the Player */
    void Load(const SharedPlaybackSequence& sequence);

    /* Sets the comparison mode for comparing the two buffers */
    void SetComparisonMode(int mode);
    void SetBlendMode(int mode);

    /* Compare Media on the Player */
    void Compare(const SharedMediaClip& first, const SharedMediaClip& second);

    /* Set a frame on the player based on the media */
    void SetFrame(int frame);

    /**
     * Updates the Handler for Missing frame
     * Setting the behaviour for when a missing frame is set
     */
    inline void SetMissingFrameHandler(int handler)
    {
        /* Update the missing frame handler */
        m_MFrameHandler = static_cast<MissingFrameHandler>(handler);
        /* And Refresh the viewport */
        Refresh();
    }

    /**
     * Sets the provided viewer buffer on the player
     */
    void SetViewBuffer(const PlayerViewBuffer& buffer);

    inline void Refresh() { SetFrame(m_Timeline->Frame()); }
    void CacheBuffer();
    void ClearCache();

    /* Zoom on the Viewport */
    inline void ZoomIn() { m_Renderer->ZoomIn(); }
    inline void ZoomOut() { m_Renderer->ZoomOut(); }
    inline void ZoomToFit() { m_Renderer->ZoomToFit(); }

    /* Mark a frame on the timeline as cached */
    inline void AddCacheFrame(int frame) { m_Timeline->AddCacheFrame(frame); }
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

public slots:
    void Clear();

private:  /* Methods */
    void Build();
    void Connect();

    /* Loads the frame from the underlying sequence */
    void SetSequenceFrame(int frame);

    /* Load the frame from a the buffer's track */
    void SetTrackFrame(int frame);

    /* Load the frame from a given track item from the sequence/track */
    void SetTrackItemFrame(SharedTrackItem item, const int frame);

    /* Loads the frame from the media in the player */
    void SetMediaFrame(int frame);

    /* Sets the Comparison Buffer frame */
    void CompareMediaFrame(v_frame_t frame);

    /**
     * Connects the signals from SharedMediaClip (i.e. shared_ptr for MediaClip)
     */
    inline void ConnectMediaClipToTimeline(const SharedMediaClip& clip)
    {
        connect(clip.get(), &MediaClip::frameCached, this, &Player::AddCacheFrame);
    }

    /**
     * Fetches the Requred settings from Prefences and sets the internal values accordingly
     */
    void SetFromPreferences();

    /**
     * Setup Annotations
     */
    void ToggleAnnotations(const bool state);

    /**
     * Add an Annotation to the Viewer Buffer
     */
    void AddAnnotation(const Renderer::SharedAnnotation& annotation);
    /**
     * Removes the Annotation from the underlying Media
     */
    void RemoveAnnotation();

private:  /* Members */
    VoidRenderer* m_Renderer;
    VoidPlaceholderRenderer* m_PlaceholderRenderer;
    Timeline* m_Timeline;

    AnnotationsController* m_AnnotationsController;

    /**
     * The control bar provides users tools to play around with the viewer
     * Providing tweak controls and how the viewer behaves
     */
    ControlBar* m_ControlBar;

    /**
     * Set a comparison Mode on the Player (of how to compare the data from the buffers)
     */
    Renderer::ComparisonMode m_ComparisonMode;
    /**
     * Set a blend mode on the Player (how to blend the data being currently compared)
     */
    Renderer::BlendMode m_BlendMode;

    /**
     * Playable Governing Component
     * Describes what is currently playing and provides information about the entity
     * Like the timerange
     */
    ViewerBuffer* m_ViewBufferA;
    ViewerBuffer* m_ViewBufferB;

    /* Describes the view buffer that is currently active and all information is queried from it */
    ViewerBuffer* m_ActiveViewBuffer;

    /* Holds the Mode of representing Missing Frames */
    MissingFrameHandler m_MFrameHandler;

    /* Internal Layout to hold Renderer -- For when the renderer has returned from it's fullscreen view */
    QVBoxLayout* m_RendererLayout;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PLAYER_WIDGET_H
