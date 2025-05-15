#ifndef _VOID_PLAYER_WIDGET_H
#define _VOID_PLAYER_WIDGET_H

/* Qt */
#include <QWidget>

/* Internal */
#include "Definition.h"
#include "Timeslider.h"
#include "VoidCore/ImageBuffer.h"
#include "VoidRenderer/Renderer.h"

VOID_NAMESPACE_OPEN

class Player : public QWidget
{
    Q_OBJECT

public:
    Player(QWidget* parent = nullptr);
    virtual ~Player();

    void Load(const VoidImageSequence& sequence);
    void SetFrame(int frame);

public slots:
    void Clear();

private:  /* Methods */
    void Build();
    void Connect();

private:  /* Members */
    VoidRenderer* m_Renderer;
    Timeslider* m_Timeslider;

    /* Media to be rendered */
    VoidImageSequence m_Sequence;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PLAYER_WIDGET_H
