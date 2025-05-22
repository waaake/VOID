#ifndef _VOID_PLAYER_WIDGET_H
#define _VOID_PLAYER_WIDGET_H

/* Qt */
#include <QWidget>

/* Internal */
#include "Definition.h"
#include "Timeline.h"
#include "VoidCore/Media.h"
#include "VoidRenderer/Renderer.h"

VOID_NAMESPACE_OPEN

class Player : public QWidget
{
    Q_OBJECT

public:
    Player(QWidget* parent = nullptr);
    virtual ~Player();

    /* Loads Playable Media on the Player */
    void Load(const Media& media);
    /* Set a frame on the player based on the media */
    void SetFrame(int frame);

public slots:
    void Clear();

private:  /* Methods */
    void Build();
    void Connect();

private:  /* Members */
    VoidRenderer* m_Renderer;
    Timeline* m_Timeline;

    /* Media to be rendered */
    Media m_Media;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PLAYER_WIDGET_H
