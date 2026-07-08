// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_H
#define _SEQUENCER_H

/* Qt */
// #include <QFrame>
#include <QLayout>
#include <QWidget>

/* Internal */
#include "Definition.h"
#include "SDescriptors.h"
#include "STimelineGeometry.h"
#include "STimelineView.h"
#include "VoidObjects/Sequence/Sequence.h"
#include "VoidUi/Sequencer/Widgets/SRulerWidget.h"
#include "VoidUi/Sequencer/Widgets/STrackHeaderWidget.h"
#include "VoidUi/Sequencer/Widgets/SToolbar.h"

VOID_NAMESPACE_OPEN

class SequencerTimeline : public QWidget
{
    Q_OBJECT
public:
    explicit SequencerTimeline(QWidget* parent = nullptr);
    void SetSequence(const SharedPlaybackSequence& sequence);
    void Refresh();

private:
    QGridLayout* m_Layout;
    SToolbar* m_Toolbar;
    SRulerWidget* m_Ruler;
    STrackHeaderWidget* m_TrackHeader;
    STimelineView* m_View;

    SharedPlaybackSequence m_Sequence;
    STimelineGeometry m_Geometry;

private: /* Methods */
    void Build();
    void Connect();
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_H
