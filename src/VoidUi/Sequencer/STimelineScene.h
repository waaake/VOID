// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_TIMELINE_SCENE_H
#define _SEQUENCER_TIMELINE_SCENE_H

/* Qt */
#include <QGraphicsScene>

/* Internal */
#include "Definition.h"
#include "SDescriptors.h"
#include "STimelineGeometry.h"
#include "VoidObjects/Sequence/Sequence.h"

VOID_NAMESPACE_OPEN

class STimelineScene : public QGraphicsScene
{
    Q_OBJECT
public:
    STimelineScene(STimelineGeometry* geometry, QObject* parent = nullptr);

    void SetSequence(const SharedPlaybackSequence& sequence);
    void Clear() { clear(); }

protected:
    void drawBackground(QPainter* painter, const QRectF& rect) override;

private:
    STimelineGeometry* m_Geometry;
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_TIMELINE_SCENE_H
