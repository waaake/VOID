#ifndef _VOID_PLAYER_WINDOW_H
#define _VOID_PLAYER_WINDOW_H

/* Qt */
#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QList>

/* Internal */
#include "About.h"
#include "Definition.h"
#include "Docker.h"
#include "MediaLister.h"
#include "PlayerWidget.h"
#include "Sequence.h"
#include "Track.h"
#include "VoidCore/Media.h"

VOID_NAMESPACE_OPEN

class VoidMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    VoidMainWindow(QWidget* parent = nullptr);
    virtual ~VoidMainWindow();

    virtual QSize sizeHint() const override;

    /* Reads Media Directory and Loads Media onto the components */
    void ImportMedia(const std::string& path);

private: /* Methods */
    void Build();
    void Connect();

protected:
    void showEvent(QShowEvent* event) override;
    
    /* Caches the Media if Caching is allowed */
    void CacheLookAhead();

    /* Clears ahe cache for the media */
    void ClearLookAheadCache();

    /*
     * Toggles the Caching behaviour for the media
     * Should the media be cache upfront?
     */
    void ToggleLookAheadCache(const bool toggle);

private: /* Members */
    VoidDocker* m_Docker;
    VoidDocker* m_MListDocker;
    QList<QDockWidget*> m_DockList;
    QList<int> m_DockSizes;

    Player* m_Player;
    VoidMediaLister* m_MediaLister;

    /* Window Menu */
    /* File Menu */
    QMenu* m_FileMenu;
    QAction* m_OpenAction;
    QAction* m_ClearAction;
    QAction* m_CloseAction;
    
    /* Playback Menu */
    QMenu* m_PlaybackMenu;
    QAction* m_EnableCacheAction;
    QAction* m_DisableCacheAction;
    QAction* m_StopCacheAction;
    QAction* m_ClearCacheAction;
    QAction* m_ResumeCacheAction;
    QAction* m_PlayForwardsAction;
    QAction* m_StopPlayingAction;
    QAction* m_PlayBackwardsAction;
    QAction* m_ForwardsAction;
    QAction* m_BackwardsAction;
    QAction* m_EndFrameAction;
    QAction* m_StartFrameAction;

    /* Help Menu */
    QMenu* m_HelpMenu;
    QAction* m_AboutAction;

    /* State determining whether to cache the current media upfront or not */
    bool m_CacheMedia;

    /* Image Sequence */
    Media m_Media;

    /* Playback Sequence holding Media entities internally */
    SharedPlaybackSequence m_Sequence;
    /* The track which gets used on the Sequence */
    SharedPlaybackTrack m_Track;

public slots:
    void Load();
    /* Clears and sets the provided media on the player */
    void SetMedia(const Media& media);
    /* Adds media onto the existing track */
    void AddMedia(const Media& media);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PLAYER_WINDOW_H
