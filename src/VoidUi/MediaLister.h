#ifndef _VOID_MEDIA_LISTER_H
#define _VOID_MEDIA_LISTER_H

/* STD */
#include <vector>

/* Qt */
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
    void AddMedia(const VoidImageSequence& media);

    /* Override the default size of the widget */
    QSize sizeHint() const override;

signals:
    void mediaChanged(const VoidImageSequence& sequence);

protected: /* Methods */
    void mousePressEvent(QMouseEvent* event) override;

private: /* Methods */
    void Build();

    /* Clears the play state of the item which was last playing, if any */
    void ClearPlaying();

    /* Clears the selected item(s), if any */
    void ClearSelection();
    
    /* Changes the selection state of the media item(s) */
    void SelectItem(VoidMediaItem* item);

    /*
     * Changes the state of the media items being played to the provided item
     * emits the mediaChanged signal with the internal media object
     */
    void ChangeMedia(VoidMediaItem* item);

private: /* Members */
    QWidget* m_Scrollwidget;
    QScrollArea* m_ScollArea;
    QVBoxLayout* m_layout;
    QVBoxLayout* m_ScrollLayout;

    std::vector<VoidMediaItem*> m_MediaItems;
    
    /* Holds the media item which is currently playing */
    VoidMediaItem* m_CurrentPlaying;

    /* Holds the media item which is currently selected */
    VoidMediaItem* m_CurrentSelectedItem;

};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_LISTER_H
