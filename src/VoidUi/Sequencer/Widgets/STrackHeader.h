// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_TRACK_HEADER_H
#define _SEQUENCER_TRACK_HEADER_H

/* Qt */
#include <QWidget>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Sequence/Track.h"
#include "VoidUi/Sequencer/SContext.h"

VOID_NAMESPACE_OPEN

class STrackHeader : public QWidget
{
    Q_OBJECT
public:
    STrackHeader(const SharedPlaybackTrack& track, SequencerContext* context, QWidget* parent = nullptr);

    QSize sizeHint() const { return QSize(Sequencer::TrackHeaderWidth, Sequencer::TrackHeight); }
    SharedPlaybackTrack Track() const { return m_Track; }

signals:
    void clicked(const SharedPlaybackTrack&);
    void lockToggled(const SharedPlaybackTrack&, bool);
    void stateToggled(const SharedPlaybackTrack&, bool);

protected:
    void paintEvent(QPaintEvent* event) override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    void resizeEvent(QResizeEvent* event) override;

private:
    SharedPlaybackTrack m_Track;
    SequencerContext* m_Context;

    QRect m_NameRect;
    QRect m_StateRect;
    QRect m_LockRect;

private: /* Methods */
    QRect LockRect() const { return m_LockRect; }
    QRect StateRect() const { return m_StateRect; }
    QRect NameRect() const { return m_NameRect; }
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_TRACK_HEADER_H
