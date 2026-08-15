// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_TRACK_ITEM_H
#define _SEQUENCER_TRACK_ITEM_H

/* STD */
#include <unordered_map>

// /* Qt */
// #include <QGraphicsObject>

/* Internal */
#include "Definition.h"
#include "STimelineItem.h"
#include "SHandleItem.h"
#include "VoidObjects/Sequence/TrackItem.h"

VOID_NAMESPACE_OPEN

// class SequencerContext;
class STrack;
class STimelineEffect;

class STrackItem : public STimelineItem
{
public:
    STrackItem(const SharedTrackItem& item, SequencerContext* context, QGraphicsItem* parent = nullptr);
    ~STrackItem();

    SharedTrackItem TrackItem() const { return m_Item; }
    bool Selected() const { return m_Context->SelectionModel()->IsSelected(m_Item); }

    STrack* Track() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void Update() override;
    void UpdateItems();

    void AddEffect(Effect* effect);
    void AddEffect(Effect* effect, int index);
    void AddEffects();
    void RemoveEffect(Effect* effect);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

private:
    std::unordered_map<Effect*, STimelineEffect*> m_Effects;
    SSlipClipContext m_SlipContext;
    SItemTrimContext m_TrimContext;
    QRectF m_HeadTrimRect, m_TailTrimRect;
    SharedTrackItem m_Item;
    SHandleItem *m_HeadHandle, *m_TailHandle, *m_DurationHandle;

private: /* Methods */
    void CalculateBoundingRect();
    void AdjustTimelineRange(v_frame_t frame);
    void AdjustEffectsWidth(double width);
    int YPos() const;
    void ToggleHandles(int head = 0, int tail = 0, int duration = 0, bool visible = false);
    QColor Background(const QStyleOptionGraphicsItem* option) const;
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_TRACK_ITEM_H
