#ifndef _VOID_PLAYER_WIDGET_H
#define _VOID_PLAYER_WIDGET_H

/* Qt */
#include <QWidget>

/* Internal */
#include "Definition.h"
#include "Timeline.h"
#include "Sequence.h"
#include "VoidCore/Media.h"
#include "VoidRenderer/Renderer.h"

VOID_NAMESPACE_OPEN

class Player : public QWidget
{
    Q_OBJECT

public:
    Player(QWidget* parent = nullptr);
    virtual ~Player();

    /* Loads Playable Media on the Player */
    void Load(const Media& media);

    /* Load a Sequence to be played on the Player */
    void Load(const SharedPlaybackSequence& sequence);

    /* Set a frame on the player based on the media */
    void SetFrame(int frame);

    /* Mark a frame on the timeline as cached */
    inline void AddCacheFrame(int frame) { m_Timeline->AddCacheFrame(frame); }

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

    /* Loads the frame from the media in the player */
    void SetMediaFrame(int frame);

private:  /* Members */
    VoidRenderer* m_Renderer;
    Timeline* m_Timeline;

    /* Media to be rendered */
    Media m_Media;

    /* Sequence to be rendered */
    SharedPlaybackSequence m_Sequence;

    /* 
     * Default VoidImageData* which can be used to read data from the Sequence
     * This pointer would get updated with data for each frame and then Render it
     */
    VoidImageData* m_Image;

    /*
     * This boolean reprents the state of playing on the Player
     * Is the Player looking at reading the Sequence of Media(s)
     * Or is the Player looking at playing the Media (which is default)
     */
    bool m_PlaySequence;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PLAYER_WIDGET_H
