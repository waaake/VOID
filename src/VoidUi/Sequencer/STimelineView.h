// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_TIMELINE_VIEW_H
#define _SEQUENCER_TIMELINE_VIEW_H

/* Qt */
#include <QGraphicsView>

/* Internal */
#include "Definition.h"
#include "FrameRange.h"
#include "SDragContext.h"
#include "VoidObjects/Sequence/Sequence.h"

VOID_NAMESPACE_OPEN

class STimelineScene;
class SequencerContext;

class STimelineView : public QGraphicsView
{
    Q_OBJECT
public:
    STimelineView(SequencerContext* m_Context, QWidget* parent = nullptr);
    void SetSequence(const SharedPlaybackSequence& sequence);
    void Clear();
    void Focus();

    /**
     * @brief The range which is currently visible in the View.
     * 
     * @return MFrameRange Visible frame range.
     */
    MFrameRange VisibleRange() const;

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void drawForeground(QPainter* painter, const QRectF& rect) override;

private:
    SequencerContext* m_Context;
    STimelineScene* m_Scene;
    SMarqueeContext m_Marquee;

private: /* Methods */
    void Build();
    void Setup();
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_TIMELINE_VIEW_H