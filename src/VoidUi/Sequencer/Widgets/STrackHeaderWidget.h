// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_TRACK_HEADER_WIDGET_H
#define _SEQUENCER_TRACK_HEADER_WIDGET_H

/* Qt */
#include <QWidget>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Sequence/Sequence.h"

VOID_NAMESPACE_OPEN

class STimelineGeometry;

class STrackHeaderWidget : public QWidget
{
    Q_OBJECT
public:
    explicit STrackHeaderWidget(STimelineGeometry* geometry, QWidget* parent = nullptr);
    void SetSequence(const SharedPlaybackSequence& sequence);
    void Clear();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    STimelineGeometry* m_Geometry;
    SharedPlaybackSequence m_Sequence;
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_TRACK_HEADER_WIDGET_H
