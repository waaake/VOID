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

class STrackHeaderItem;
class SHeaderScene;
class SequencerContext;
class SNameEditor;

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
    
    void EditTrackName();
    STrackHeaderItem* Header(const SharedPlaybackTrack& track) const;
    QPoint TagPos(const SharedPlaybackTrack& track) const;

private:
    SequencerContext* m_Context;
    SHeaderScene* m_Scene;

    SNameEditor* m_NameEditor;

private: /* Methods */
    void Build();
    void Setup();
    void AcceptEdit(const QString& text);
    void EditCancelled();
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_TRACK_HEADER_VIEW_H
