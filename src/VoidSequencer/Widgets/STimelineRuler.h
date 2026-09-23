// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_TIMELINE_RULER_H
#define _SEQUENCER_TIMELINE_RULER_H

/* Qt */
#include <QWidget>

/* Internal */
#include "Definition.h"
#include "VoidCore/Timekeeper.h"

VOID_NAMESPACE_OPEN

class STimelineView;
class SequencerContext;

class STimelineRuler : public QWidget
{
public:
    STimelineRuler(STimelineView* view, SequencerContext* context, QWidget* parent = nullptr);
    void Update() { update(); }

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    const Timekeeper& m_Timekeeper;
    STimelineView* m_View;
    SequencerContext* m_Context;
    bool m_Pressed;

private: /* Methods */
    int TimeDisplayInterval(const double pixelsPerFrame) const;
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_TIMELINE_RULER_H
