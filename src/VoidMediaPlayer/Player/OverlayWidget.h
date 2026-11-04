// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_PLAYER_OVERLAY_WIDGET
#define _VOID_PLAYER_OVERLAY_WIDGET

/* Qt */
#include <QWidget>

/* Internal */
#include "QDefinition.h"

VOID_NAMESPACE_OPEN

class PlayerOverlay : public QWidget
{
public: /* Enums */
    enum class HoveredViewerBuffer
    {
        A,
        B,
        None
    };

public:
    PlayerOverlay(QWidget* parent = nullptr);

    void ResetRect();
    void SetHoveredBuffer(const QPoint& pos);
    void SetHoveredBuffer(const HoveredViewerBuffer& buffer);

    inline HoveredViewerBuffer HoveredBuffer() const { return m_HoveredViewerBuffer; }

protected:
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private: /* Members */
    bool m_MediaDragHighlight;
    HoveredViewerBuffer m_HoveredViewerBuffer;

    QRect m_RectA;
    QRect m_RectB;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PLAYER_OVERLAY_WIDGET
