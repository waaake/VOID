#ifndef _VOID_MEDIA_ITEM_H
#define _VOID_MEDIA_ITEM_H

/* Qt */
#include <QLabel>
#include <QLayout>
#include <QWidget>

/* Internal */
#include "Definition.h"
#include "VoidCore/Media.h"

VOID_NAMESPACE_OPEN

class VoidMediaItem : public QFrame
{
    Q_OBJECT

public:
    VoidMediaItem(const Media& media, QWidget* parent = nullptr);
    virtual ~VoidMediaItem();

    void Update();

    /* Returns whether the media item is currently selected? */
    bool Selected() const { return m_Selected; }
    
    /* Returns the Media associated with the Item */
    Media Clip() const { return m_Clip; }

    /* Returns whether the item is being played/displayed/rendered on the renderer */
    bool Playing() const { return m_Playing; }

    /* Sets the Playing state on the Media */
    void SetPlaying(bool play);

    /* Sets the selection state on the media item */
    void SetSelected(bool selected);

signals:
    void clicked(VoidMediaItem*);
    void selected(VoidMediaItem*, bool);
    void doubleClicked(VoidMediaItem*);

private: /* Methods */
    void Build();
    void Setup();

    /* Returns formatted framerate as 24.0fps */
    std::string GetFramerate() const;

    /* 
     * Returns formatted frame range of the sequence/Media
     * Example: 1001-1010
     */
    std::string GetRange() const;

    /* Returns the Name of the Media */
    std::string GetName() const;

    /* Returns the type of the media */
    std::string GetType() const;

    /* Returns the first frame of the media as a QPixmap which is scaled to standard size */
    QPixmap GetThumbnail() const;

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private: /* Members */
    /* State for the widget selection */
    bool m_Selected;
    /* State for media playing */
    bool m_Playing;

    /* Media */
    Media m_Clip;

    /* Layouts */
    QHBoxLayout* m_Layout;
    QVBoxLayout* m_RtLayout;
    QHBoxLayout* m_IntTpLayout; /* Top Internal Layout */
    QHBoxLayout* m_IntBtLayout; /* Bottom Internal Layout */
    QVBoxLayout* m_IconLayout; /* Right layout for displaying icons */

    /* Widgets */
    QLabel* m_NameLabel;
    QLabel* m_RangeLabel;
    QLabel* m_FramerateLabel;
    QLabel* m_TypeLabel;
    QLabel* m_PlayLabel;

    QLabel* m_Thumbnail;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_ITEM_H