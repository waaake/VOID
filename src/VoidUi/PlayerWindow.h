#ifndef _VOID_PLAYER_WINDOW_H
#define _VOID_PLAYER_WINDOW_H

/* Qt */
#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QList>

/* Internal */
#include "Definition.h"
#include "Docker.h"
#include "MediaLister.h"
#include "PlayerWidget.h"
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
    void ReadDirectory(const std::string& path);

private: /* Methods */
    void Build();
    void Connect();

protected:
    void showEvent(QShowEvent* event) override;
    
    /* Caches the Media if Caching is allowed */
    void CacheLookAhead();

    /* Clears ahe cache for the media */
    void ClearLookAheadCache();

private: /* Members */
    VoidDocker* m_Docker;
    VoidDocker* m_MListDocker;
    QList<QDockWidget*> m_DockList;
    QList<int> m_DockSizes;

    Player* m_Player;
    VoidMediaLister* m_MediaLister;

    /* Window Menu */
    QMenu* m_FileMenu;
    QAction* m_OpenAction;
    QAction* m_ClearAction;
    QAction* m_ClearCacheAction;
    QAction* m_CloseAction;

    /* State determining whether to cache the current media upfront or not */
    bool m_CacheMedia;

    /* Image Sequence */
    Media m_Media;

public slots:
    void Load();
    void SetMedia(const Media& media);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PLAYER_WINDOW_H
