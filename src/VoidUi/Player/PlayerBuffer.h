// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _PLAYER_BUFFER_H
#define _PLAYER_BUFFER_H

/* Qt */
#include <QObject>

/* Internal */
#include "Definition.h"
#include "Components.h"
#include "VoidObjects/Sequence/Track.h"
#include "VoidObjects/Sequence/Sequence.h"
#include "VoidObjects/Media/MediaClip.h"
#include "VoidObjects/Playlist/Playlist.h"

VOID_NAMESPACE_OPEN

class VOID_API PlayerBuffer : public QObject
{
    Q_OBJECT
public:
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
        Clip,
        Playlist,
        Grid
    };

public:
    PlayerBuffer(QObject* parent = nullptr);

    /// Attribs

    inline v_frame_t StartFrame() const { return m_Startframe; }
    inline v_frame_t EndFrame() const { return m_Endframe; }
    inline PlayableComponent PlayingComponent() const { return m_PlayingComponent; }

    /// Entities

    SharedPlaybackTrack ActiveTrack() const;
    SharedTrackItem TrackItem(const v_frame_t frame);
    SharedMediaClip Media(const v_frame_t frame);
    inline SharedMediaClip GetMediaClip() const { return m_Clip; }
    inline SharedPlaybackTrack GetTrack() const { return m_Track; }
    inline SharedPlaybackSequence GetSequence() const { return m_Sequence; }

    /// ImageData

    BufferData MData(const v_frame_t frame, bool nearest = false);
    std::vector<FloatImage> GridFrame(const v_frame_t frame);

    bool Playing(const SharedMediaClip& media) const;

    /// Media

    [[nodiscard]] bool NextMedia();
    [[nodiscard]] bool PreviousMedia();
    [[nodiscard]] bool ResetPlaylistMedia();

    /// Set Media

    void Set(const SharedMediaClip& media);
    void Set(const SharedPlaybackTrack& track);
    void Set(const SharedPlaybackSequence& sequence);
    void Set(const std::vector<SharedMediaClip>& media);
    void SetGrid(Playlist* playlist);
    void SetPlaylist(Playlist* playlist);

    /// Media Components

    void SetAnnotation(const v_frame_t, const Renderer::SharedAnnotation& annotation);
    void RemoveAnnotation(const v_frame_t);
    const std::unordered_map<v_frame_t, Renderer::SharedAnnotation>& Annotations() const { return m_Clip->Annotations(); }

signals:
    void updated();
    void mediaAboutToBeUpdated();
    void mediaUpdated();
    void rangeUpdated(v_frame_t, v_frame_t);
    void playlistUpdated(Playlist*);

protected: /* Members */
    SharedMediaClip m_Clip;
    SharedPlaybackTrack m_Track;
    SharedPlaybackSequence m_Sequence;
    Playlist* m_Playlist;
    v_frame_t m_Startframe, m_Endframe;
    PlayableComponent m_PlayingComponent;

protected: /* Methods */
    inline v_frame_t InternalStartframe() const { return m_Startframe; }
    inline v_frame_t InternalEndframe() const { return m_Endframe; }
    inline int InternalDuration() const { return m_Endframe - m_Startframe + 1; }
    void ResetMedia();
    void UpdateRange(v_frame_t start, v_frame_t end);
};

VOID_NAMESPACE_CLOSE

#endif // _PLAYER_BUFFER_H
