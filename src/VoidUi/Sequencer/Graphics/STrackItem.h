// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_TRACK_ITEM_H
#define _SEQUENCER_TRACK_ITEM_H

/* Qt */
#include <QGraphicsObject>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Sequence/TrackItem.h"

VOID_NAMESPACE_OPEN

class STimelineGeometry;

class STrackItem : public QGraphicsObject
{
    Q_OBJECT
public:
    STrackItem(const SharedTrackItem& item, STimelineGeometry* geometry, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    void Update();

private:
    SharedTrackItem m_Item;
    STimelineGeometry* m_Geometry;
    QRectF m_BoundingRect;

private: /* Methods */
    void CalculateBoundingRect();
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_TRACK_ITEM_H
