// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_TRACK_HEADER_WIDGET_H
#define _SEQUENCER_TRACK_HEADER_WIDGET_H

/* Qt */
#include <QLayout>
#include <QScrollArea>
#include <QWidget>

/* Internal */
#include "Definition.h"
#include "VoidComponents/Widget.h"
#include "VoidObjects/Sequence/Sequence.h"

VOID_NAMESPACE_OPEN

class SequencerContext;

class STrackHeaderWidget : public QWidget, public IWidget
{
    Q_OBJECT
public:
    explicit STrackHeaderWidget(SequencerContext* context, QWidget* parent = nullptr);

    void DeleteSelected() override;
    void AddTrack(const SharedPlaybackTrack& track);
    void RemoveTrack(const SharedPlaybackTrack& track);
    void Clear();

    void Update();
    void SetScroll(int value);

signals:
    void deleteSelectionRequested();

private:
    QVBoxLayout* m_Layout;
    QVBoxLayout* m_ScrollLayout;
    QScrollArea* m_ScrollArea;
    SequencerContext* m_Context;

private: /* Methods */
    void Build();
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_TRACK_HEADER_WIDGET_H
