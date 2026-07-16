// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_TIMELINE_RULER_H
#define _SEQUENCER_TIMELINE_RULER_H

/* Qt */
#include <QWidget>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class STimelineView;
class SequencerContext;

class STimelineRuler : public QWidget
{
    Q_OBJECT
public:
    STimelineRuler(STimelineView* view, SequencerContext* context, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    STimelineView* m_View;
    SequencerContext* m_Context;
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_TIMELINE_RULER_H
