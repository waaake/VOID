// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _PLAYER_BRIDGE_H
#define _PLAYER_BRIDGE_H

/* Qt */
#include <QObject>

/* Internal */
#include "Definition.h"
#include "Player.h"
#include "VoidUi/BaseWindow/MenuSystem.h"

VOID_NAMESPACE_OPEN

class VOID_API PlayerBridge : public QObject
{
    PlayerBridge();
public:
    ~PlayerBridge();

    PlayerBridge(const PlayerBridge&) = delete;
    PlayerBridge(PlayerBridge&&) = delete;

    PlayerBridge& operator=(const PlayerBridge&) = delete;
    PlayerBridge& operator=(PlayerBridge&&) = delete;

    static PlayerBridge& Instance();
    inline Player* ActivePlayer() const { return m_Player; }
    void InitMenu(MenuSystem* menuSystem);
    inline void Clear() { m_Player->Clear(); }

    inline void SetMedia(const SharedMediaClip& media) { m_Player->SetMedia(media); }
    inline void SetMedia(const SharedMediaClip& media, const PlayerViewBuffer& buffer)
    {
        m_Player->SetMedia(media, buffer);
    }
    inline void SetMedia(const std::vector<SharedMediaClip>& media) { m_Player->SetMedia(media); }
    inline void SetMedia(const std::vector<SharedMediaClip>& media, const PlayerViewBuffer& buffer)
    {
        m_Player->SetMedia(media, buffer);
    }

    inline void SetPlaylist(Playlist* playlist) { m_Player->SetPlaylist(playlist); }
    inline void ClearQueue() { m_Playlist->Clear(); }
    void AddToQueue(const SharedMediaClip& media, bool refresh = true);
    void AddToQueue(const std::vector<SharedMediaClip>& media, bool refresh = true);

    inline void ResumeCache() { m_Player->ResumeCache(); }
    inline void DisableCache() { m_Player->DisableCache(); }
    inline void StopCache() { m_Player->StopCache(); }
    inline void Recache() { m_Player->Recache(); }
    inline void ClearCache() { m_Player->ClearCache(); }

    inline void PlayForwards() { m_Player->PlayForwards(); }
    inline void PlayBackwards() { m_Player->PlayBackwards(); }
    inline void Stop() { m_Player->Stop(); }

    inline void NextFrame() { m_Player->NextFrame(); }
    inline void PreviousFrame() { m_Player->PreviousFrame(); }
    inline void MoveToStart() { m_Player->MoveToStart(); }
    inline void MoveToEnd() { m_Player->MoveToEnd(); }
    inline void ResetInFrame() { m_Player->ResetInFrame(); }
    inline void ResetOutFrame() { m_Player->ResetOutFrame(); }
    inline void ResetRange() { m_Player->ResetRange(); }

    inline void ZoomIn() { m_Player->ZoomIn(); }
    inline void ZoomOut() { m_Player->ZoomOut(); }
    inline void ZoomToFit() { m_Player->ZoomToFit(); }
    inline void SetFullscreen() { m_Player->SetRendererFullscreen(); }
    inline void ExitFullscreen() { m_Player->ExitFullscreenRenderer(); }

private: /* Members */
    Player* m_Player;
    Playlist* m_Playlist;
};

#define _PlayerBridge PlayerBridge::Instance()

VOID_NAMESPACE_CLOSE

#endif // _PLAYER_BRIDGE_H
