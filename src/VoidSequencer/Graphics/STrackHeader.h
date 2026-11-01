// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_GRAPHICS_TRACK_HEADER_H
#define _SEQUENCER_GRAPHICS_TRACK_HEADER_H

/* Internal */
#include "Definition.h"
#include "STimelineItem.h"
#include "VoidObjects/Sequence/Track.h"

VOID_NAMESPACE_OPEN

class STrackHeaderItem : public STimelineItem
{
public:
    STrackHeaderItem(const SharedPlaybackTrack& track, SequencerContext* context, QGraphicsItem* parent = nullptr);

    SharedPlaybackTrack& Track() { return m_Track; }
    const SharedPlaybackTrack& Track() const { return m_Track; }
    int Index() const { return m_Track->Index(); }
    bool Locked() const { return m_Track->Locked(); }
    bool Enabled() const { return m_Track->Enabled(); }

    void Update() override;

    QRect NameRect() const { return m_NameRect; }
    QRect TagRect() const { return m_TagRect; }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private:
    QRect m_NameRect;
    QRect m_StateRect;
    QRect m_LockRect;
    QRect m_TagRect;

protected:
    SharedPlaybackTrack m_Track;

private: /* Methods */
    void Connect();
    void Resize(int index);

protected:
    QRect LockRect() const { return m_LockRect; }
    QRect StateRect() const { return m_StateRect; }
};

class SVideoTrackHeaderItem : public STrackHeaderItem
{
public:
    using STrackHeaderItem::STrackHeaderItem;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    QColor Background(const QPalette& palette) const;
};

class SAudioTrackHeaderItem : public STrackHeaderItem
{
public:
    using STrackHeaderItem::STrackHeaderItem;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    QColor Background(const QPalette& palette) const;
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_GRAPHICS_TRACK_HEADER_H
