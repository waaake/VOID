#ifndef _VOID_BUFFER_SWITCH_H
#define _VOID_BUFFER_SWITCH_H

/* Qt */
#include <QLayout>
#include <QFrame>
// #include <QLabel>
// #include <QLineEdit>

/* Internal */
#include "Definition.h"
#include "ViewerBuffer.h"

VOID_NAMESPACE_OPEN

class BufferPage : public QFrame
{
    Q_OBJECT

public:
    BufferPage(ViewerBuffer* viewBuffer, const PlayerViewBuffer& buffer, QWidget* parent = nullptr);
    ~BufferPage();

signals:
    void selected(const PlayerViewBuffer&);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private: /* Members */
    ViewerBuffer* m_ViewerBuffer;

    /* The Buffer related to this Switch */
    PlayerViewBuffer m_Buffer;
};

class BufferSwitch : public QWidget
{
    Q_OBJECT

public:
    BufferSwitch(ViewerBuffer* A, ViewerBuffer* B, QWidget* parent = nullptr);
    ~BufferSwitch();

signals:
    void switched(const PlayerViewBuffer&);

private: /* Members */
    /* Main Layout */
    QHBoxLayout* m_Layout;

    /* Buffer Pages */
    BufferPage* m_BufferA;
    BufferPage* m_BufferB;

    /* Buffers */
    ViewerBuffer* m_ViewerBufferA;
    ViewerBuffer* m_ViewerBufferB;

private: /* Methods */
    /* Builds the UI */
    void Build();
    /* Connects Signals */
    void Connect();

};

VOID_NAMESPACE_CLOSE

#endif // _VOID_BUFFER_SWITCH_H