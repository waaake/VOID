// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_TRACK_HEADER_VIEW_H
#define _SEQUENCER_TRACK_HEADER_VIEW_H

/* Qt */
#include <QGraphicsView>

/* Internal */
#include "QDefinition.h"
#include "VoidSequencer/SDragContext.h"
#include "VoidObjects/Sequence/Sequence.h"

VOID_NAMESPACE_OPEN

// class STimelineScene;
class SHeaderScene;
class SequencerContext;

class SHeaderView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit SHeaderView(SequencerContext* m_Context, QWidget* parent = nullptr);
    ~SHeaderView();
    void AddPlayhead();
    void AddVideoTrack(const SharedPlaybackTrack& track);
    void AddAudioTrack(const SharedPlaybackTrack& track);
    void RemoveTrack(const SharedPlaybackTrack& track);
    void Refresh();
    void Clear();
    void ResetScroll();
    void SetScroll(int value);

private:
    SequencerContext* m_Context;
    SHeaderScene* m_Scene;

private: /* Methods */
    void Build();
    void Setup();
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_TRACK_HEADER_VIEW_H
