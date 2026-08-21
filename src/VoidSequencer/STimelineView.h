// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_TIMELINE_VIEW_H
#define _SEQUENCER_TIMELINE_VIEW_H

/* Qt */
#include <QGraphicsView>

/* Internal */
#include "QDefinition.h"
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
    void AddPlayhead();
    void AddTrack(const SharedPlaybackTrack& track);
    void RemoveTrack(const SharedPlaybackTrack& track);
    void Refresh();
    void Clear();
    void Focus();

    /**
     * @brief The range which is currently visible in the View.
     * 
     * @return MFrameRange Visible frame range.
     */
    MFrameRange VisibleRange() const;

signals:
    void sequenceCutRequested(v_frame_t);

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void drawForeground(QPainter* painter, const QRectF& rect) override;
    void enterEvent(EnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

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
