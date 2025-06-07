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
#include "FramelessWindow.h"
#include "MediaLister.h"
#include "PlayerWidget.h"
#include "Sequence.h"
#include "TitleBar.h"
#include "Track.h"
#include "VoidCore/Media.h"

VOID_NAMESPACE_OPEN

class DockerWindow : public QMainWindow
{
public: /* Enum */
    enum class Component
    {
        MediaLister,
    };

public:
    explicit DockerWindow(QWidget* parent = nullptr);
    virtual ~DockerWindow();

    /* Component Getters */
    /* Returns the Pointer to Player Widget */
    Player* GetPlayer() const { return m_Player; }
    /* Returns the Pointer to the Media Lister Widget */
    VoidMediaLister* MediaLister() const { return m_MediaLister; }

    void ToggleComponent(const Component& component, const bool state);

private: /* Members */
    VoidDocker* m_Docker;
    VoidDocker* m_MListDocker;
    QList<QDockWidget*> m_DockList;
    QList<int> m_DockSizes;

    /* VOID Components */
    Player* m_Player;
    VoidMediaLister* m_MediaLister;

private: /* Methods */
    void Build();
    void ToggleDock(VoidDocker* dock, const bool state, const Qt::DockWidgetArea& area);

};

class VoidMainWindow : public FramelessWindow
{
    Q_OBJECT

public:
    VoidMainWindow(QWidget* parent = nullptr);
    virtual ~VoidMainWindow();

    virtual QSize sizeHint() const override;

    /* Reads Media Directory and Loads Media onto the components */
    void ImportMedia(const std::string& path);
    void PlayMedia(const std::vector<Media>& items);

    SharedPlaybackSequence ActiveSequence() const { return m_Sequence; }

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
    Player* m_Player;
    VoidMediaLister* m_MediaLister;

    DockerWindow* m_InternalDocker;

    VoidTitleBar* m_TitleBar;

    /* Window Menu */
    QLabel* m_VoidMenuIcon;

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

    /* Viewer Menu */
    QMenu* m_ViewerMenu;
    QAction* m_ZoomInAction;
    QAction* m_ZoomOutAction;
    QAction* m_ZoomToFitAction;

    /* Window Menu */
    QMenu* m_WindowMenu;
    QAction* m_MediaListerAction;

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
