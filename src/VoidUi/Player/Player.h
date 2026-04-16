// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_PLAYER_H
#define _VOID_PLAYER_H

/* Internal */
#include "PlayerWidget.h"
#include "VoidAudio/Core/Decoder.h"

VOID_NAMESPACE_OPEN

class VOID_API Player : public PlayerWidget
{
    Q_OBJECT

public:
    Player(QWidget* parent = nullptr);
    ~Player();
    
    virtual inline QSize sizeHint() const override { return QSize(640, 480); }

    /**
     * @brief Set the Frame on the Player Timeline.
     * 
     * @param frame Framenumber to be set on the Timeline.
     */
    void SetFrame(int frame);

    inline void Refresh() { SetFrame(m_Timeline->Frame()); }
    inline ViewerBuffer* ActiveViewer() const { return m_ActiveViewBuffer; }

    void SetMedia(const SharedMediaClip& media);
    void SetMedia(const std::vector<SharedMediaClip>& media);
    void SetMedia(const std::vector<SharedMediaClip>& media, const PlayerViewBuffer& buffer);
    void SetMedia(const SharedMediaClip& media, const PlayerViewBuffer& buffer);
    void SetTrack(const SharedPlaybackTrack& track);
    void SetTrack(const SharedPlaybackTrack& track, const PlayerViewBuffer& buffer);
    void SetSequence(const SharedPlaybackSequence& sequence);
    void SetPlaylist(Playlist* playlist);

    void ToggleChannels(int channel);
    void SetBlendMode(int mode);
    void SetComparisonMode(int mode);
    /* Compare Media on the Player */
    void Compare(const SharedMediaClip& first, const SharedMediaClip& second);
    void InspectCurrentMetadata();

    void PauseCache();
    void DisableCache();
    void StopCache();
    void Recache();
    void ResumeCache();
    void ClearCache();

signals:
    /**
     * Emitted when metadata is inspected for a Media Clip, probably being played
     * @param clip: The Shared pointer to the Media clip for which the metadata is to be inspected.
     */
    void metadataInspected(const SharedMediaClip&);

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private: /* Members */
    AudioDecoder m_AudioDecoder;

private: /* Methods */
    void Render(int frame);
    // void SetMediaFrame(int frame);
    // void SetTrackFrame(int frame);
    // void SetSequenceFrame(int frame);
    // void SetTrackItemFrame(SharedTrackItem item, const int frame);
    void CompareMediaFrame(v_frame_t frame);

    void ResetViewBuffer(const PlayerViewBuffer& buffer);

    void Connect();

    void PreviousMedia();
    void NextMedia();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PLAYER_H
