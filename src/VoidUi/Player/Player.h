// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_PLAYER_H
#define _VOID_PLAYER_H

/* Internal */
#include "PlayerWidget.h"

VOID_NAMESPACE_OPEN

class VOID_API Player : public PlayerWidget
{
public:
    Player(QWidget* parent = nullptr);
    ~Player();

    void SetFrame(int frame);

    inline void Refresh() { SetFrame(m_Timeline->Frame()); }

    void SetMedia(const SharedMediaClip& media);
    void SetMedia(const std::vector<SharedMediaClip>& media);
    void SetMedia(const std::vector<SharedMediaClip>& media, const PlayerViewBuffer& buffer);
    void SetMedia(const SharedMediaClip& media, const PlayerViewBuffer& buffer);
    void SetTrack(const SharedPlaybackTrack& track);
    void SetTrack(const SharedPlaybackTrack& track, const PlayerViewBuffer& buffer);
    void SetSequence(const SharedPlaybackSequence& sequence);
    void SetPlaylist(Playlist* playlist);

    void SetBlendMode(int mode);
    void SetComparisonMode(int mode);
    /* Compare Media on the Player */
    void Compare(const SharedMediaClip& first, const SharedMediaClip& second);

    inline void PauseCache() { m_CacheProcessor.PauseCaching(); }
    inline void DisableCache() { m_CacheProcessor.DisableCaching(); }
    inline void StopCache() { m_CacheProcessor.StopCaching(); }
    inline void Recache() { m_CacheProcessor.Recache(); }
    inline void ResumeCache() { m_CacheProcessor.ResumeCaching(); }
    inline void ClearCache() { m_CacheProcessor.ClearCache(); }

    void StartAudio();
    void StopAudio();

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private: /* Members */
    ChronoFlux m_CacheProcessor;

private: /* Methods */
    void SetMediaFrame(int frame);
    void SetTrackFrame(int frame);
    void SetSequenceFrame(int frame);
    void SetTrackItemFrame(SharedTrackItem item, const int frame);
    void CompareMediaFrame(v_frame_t frame);

    void ResetViewBuffer(const PlayerViewBuffer& buffer);

    void Connect();
    void ResetCacheMedia();

    void PreviousMedia();
    void NextMedia();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PLAYER_H
