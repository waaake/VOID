#ifndef _VOID_PLAYER_WIDGET_H
#define _VOID_PLAYER_WIDGET_H

/* Qt */
#include <QWidget>

/* Internal */
#include "ControlBar.h"
#include "Definition.h"
#include "MediaClip.h"
#include "Timeline.h"
#include "Sequence.h"
#include "VoidCore/Media.h"
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
        /* Since the sequence is currently being played -- This does not have an active media clip element */
        if (m_PlaySequence)
            return nullptr;
        
        /* Return the active media clip */
        return m_MediaClip;
    }

    /* Loads a Playable Media (clip) on the Player */
    void Load(const SharedMediaClip& media);

    /* Load a Sequence to be played on the Player */
    void Load(const SharedPlaybackSequence& sequence);

    /* Set a frame on the player based on the media */
    void SetFrame(int frame);

    /*
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

    inline void Refresh() { SetFrame(m_Timeline->Frame()); }

    /* Zoom on the Viewport */
    inline void ZoomIn() { m_Renderer->ZoomIn(); }
    inline void ZoomOut() { m_Renderer->ZoomOut(); }
    inline void ZoomToFit() { m_Renderer->ZoomToFit(); }

    /* Mark a frame on the timeline as cached */
    inline void AddCacheFrame(int frame) { m_Timeline->AddCacheFrame(frame); }
    inline void ClearCachedFrames() { m_Timeline->ClearCachedFrames(); }

    /* Set Range on the timeline */
    inline void SetRange(int start, int end) { m_Timeline->SetRange(start, end); }

    /*
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

public slots:
    void Clear();

private:  /* Methods */
    void Build();
    void Connect();

    /* Loads the frame from the underlying sequence */
    void SetSequenceFrame(int frame);

    /* Load the frame from a given track item from the sequence/track */
    void SetTrackItemFrame(SharedTrackItem item, const int frame);

    /* Loads the frame from the media in the player */
    void SetMediaFrame(int frame);

private:  /* Members */
    VoidRenderer* m_Renderer;
    Timeline* m_Timeline;

    /*
     * The control bar provides users tools to play around with the viewer
     * Providing tweak controls and how the viewer behaves
     */
    ControlBar* m_ControlBar;

    /* Media Clip to be played/rendered */
    SharedMediaClip m_MediaClip;

    /* Sequence to be rendered */
    SharedPlaybackSequence m_Sequence;

    /*
     * This boolean reprents the state of playing on the Player
     * Is the Player looking at reading the Sequence of Media(s)
     * Or is the Player looking at playing the Media (which is default)
     */
    bool m_PlaySequence;

    /* Holds the Mode of representing Missing Frames */
    MissingFrameHandler m_MFrameHandler;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PLAYER_WIDGET_H
