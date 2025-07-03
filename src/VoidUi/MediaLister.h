#ifndef _VOID_MEDIA_LISTER_H
#define _VOID_MEDIA_LISTER_H

/* STD */
#include <vector>
#include <unordered_map>

/* Qt */
#include <QAction>
#include <QPushButton>
#include <QWidget>
#include <QScrollArea>
#include <QLayout>

/* Internal */
#include "Definition.h"
#include "MediaItem.h"

VOID_NAMESPACE_OPEN

class VoidMediaLister : public QWidget
{
    Q_OBJECT

public:
    VoidMediaLister(QWidget* parent = nullptr);
    virtual ~VoidMediaLister();

    /* Adds a Media Item to the List of widgets based on the provided Media */
    void AddMedia(const SharedMediaClip& media);
    void RemoveMedia(const SharedMediaClip& media);

    /* Override the default size of the widget */
    QSize sizeHint() const override;

signals:
    void mediaChanged(const SharedMediaClip& media);
    /* For a bunch of media is set to be played */
    void playlistChanged(const std::vector<SharedMediaClip>& media);
    void mediaDropped(const std::string& path);

protected: /* Methods */
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

private: /* Methods */
    void Build();

    /* Setup how the default UI elements appear */
    void Setup();

    /* Connects Signals across the componets of the widget */
    void Connect();

    /* Clears the play state of the item which was last playing, if any */
    void ClearPlaying();

    /* Clears the selected item(s), if any */
    void ClearSelection();
    
    /* Changes the selection state of the media item(s) */
    void SelectItem(VoidMediaItem* item, bool clear = true);

    /*
     * Changes the state of the media items being played to the provided item
     * emits the mediaChanged signal with the internal media object
     */
    void ChangeMedia(VoidMediaItem* item);

    void AddSelectionToSequence();
    void RemoveSelectedMedia();

private: /* Members */
    QWidget* m_Scrollwidget;
    QScrollArea* m_ScollArea;
    QVBoxLayout* m_layout;
    QVBoxLayout* m_ScrollLayout;
    QHBoxLayout* m_OptionsLayout;

    /* Options */
    QPushButton* m_DeleteButton;

    /* Context Menu */
    QAction* m_PlayAction;
    QAction* m_RemoveAction;

    std::unordered_map<std::string, VoidMediaItem*> m_MediaItems;
    
    /* Holds the media item which is currently playing */
    std::vector<VoidMediaItem*> m_CurrentPlaying;

    /* Holds the media item which is currently selected */
    std::vector<VoidMediaItem*> m_CurrentSelected;

};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_LISTER_H
