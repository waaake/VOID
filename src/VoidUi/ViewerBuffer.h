#ifndef _VOID_VIEWER_BUFFER_H
#define _VOID_VIEWER_BUFFER_H

/* Qt */
#include <QColor>
#include <QObject>

/* Internal */
#include "Definition.h"
#include "MediaClip.h"
#include "Track.h"
#include "Sequence.h"

VOID_NAMESPACE_OPEN

/**
 * Enum decribing which viewer buffer is currently active and can be used to set an
 * active viewer buffer for the 
 */
enum class PlayerViewBuffer
{
    /* Relates to ViewerBuffer A */
    A,
    /* Relates to ViewerBuffer B */
    B

    /* In Future the Buffer type may also include Compare buffers like switch/swipe and others. */
};

class ViewerBuffer : public QObject
{
    Q_OBJECT

public: /* Enums */
    /**
     * Describes the playing components on the Buffer
     * The components are Media Clip
     * Track from a Sequence
     * The entire sequence which means all tracks are included and their priority
     * are controlled by the sequence itself
     */
    enum class PlayableComponent
    {
        Sequence,
        Track,
        Clip
    };

public:
    ViewerBuffer(const std::string& name, QObject* parent = nullptr);
    ViewerBuffer(QObject* parent = nullptr);
    ~ViewerBuffer();

    SharedPlaybackTrack ActiveTrack() const;

    /**
     * Range for the buffer
     */
    inline int StartFrame() const { return m_Startframe; }
    inline int EndFrame() const { return m_Endframe; }

    /**
     * Returns the current Component Type which is playing in the buffer
     */
    inline PlayableComponent PlayingComponent() const { return m_PlayingComponent; }

    /**
     * Returns a track item at a given frame
     */
    inline SharedTrackItem TrackItem(const int frame) const
    {
        /* Return from the sequence if that is currently playing */
        if (m_PlayingComponent == PlayableComponent::Sequence)
            return m_Sequence->GetTrackItem(frame);

        /* Else return from the track */
        return m_Track->GetTrackItem(frame);
    }

    /**
     * Returns the shared media clip instance
     */
    inline SharedMediaClip GetMediaClip() const { return m_Clip; }

    /**
     * Returns the shared media track instance
     */
    inline SharedPlaybackTrack GetTrack() const { return m_Track; }
    
    /**
     * Returns the shared sequence instance
     */
    inline SharedPlaybackSequence GetSequence() const { return m_Sequence; }

    /**
     * Active state of the Viewer Buffer
     */
    [[nodiscard]] inline bool Active() const { return m_Active; }
    inline void SetActive(const bool active) { m_Active = active; emit updated();}

    /**
     * Name of the viewer buffer
     */
    inline std::string Name() const { return m_Name; }
    inline void SetName(const std::string& name) { m_Name = name; }

    /**
     * Color Associtated with the viewer buffer to indicate its reference across the UI
     */
    inline QColor Color() const { return m_Color; }
    void SetColor(const QColor& color);

    /**
     * Set a playable component on the Buffer
     */
    void Set(const SharedMediaClip& media);
    void Set(const SharedPlaybackTrack& track);
    void Set(const SharedPlaybackSequence& sequence);

signals:
    /* The updated signal is emitted when the viewer buffer receives a change */
    void updated();

private: /* Members */
    /**
     * Playable Entities 
     * Obviously one will get played at a time but can be governed what gets played
     */
    SharedMediaClip m_Clip;
    SharedPlaybackTrack m_Track;
    SharedPlaybackSequence m_Sequence;

    /* Currently playing component */
    PlayableComponent m_PlayingComponent;

    /* Frame range of the playing component */
    int m_Startframe, m_Endframe;

    /* Name of the Viewer Buffer */
    std::string m_Name;

    /* Color associated with the Buffer to indicate where possible */
    QColor m_Color;

    /** 
     * Represents the state of the Buffer
     * Whether the buffer is active or not at the moment
     */
    bool m_Active;

};

VOID_NAMESPACE_CLOSE

#endif // _VOID_VIEWER_BUFFER_H
