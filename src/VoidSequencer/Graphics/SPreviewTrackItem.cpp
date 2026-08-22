// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>

/* Internal */
#include "SPreviewTrackItem.h"
#include "STrack.h"

VOID_NAMESPACE_OPEN

SPreviewTrackItem::SPreviewTrackItem(const SharedTrackItem& item, SequencerContext* context, QGraphicsItem* parent)
    : STimelineItem(context, parent)
    , m_Item(item)
{
    setZValue(Sequencer::ZMarker);

    CalculateBoundingBox();
    setPos(context->Geometry()->FrameToSceneX(item->TimelineIn()), 0);
    setOpacity(0.6);
}

void SPreviewTrackItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->setRenderHint(QPainter::Antialiasing);
    const int width = boundingRect().width();

    const QColor color = IsObstructed() ? QColor(220, 40, 50) : m_Item->Color();

    painter->setPen(QPen(color, 1));
    painter->setBrush(color.darker(250));
    painter->drawRect(boundingRect());

    painter->fillRect(2, 2, std::min(6, width - 2) , Sequencer::TrackItemHeight - 8, color);

    painter->setPen(option->palette.color(QPalette::Text));
    painter->drawText(boundingRect().adjusted(10, 0, -2, 0), Qt::AlignLeft | Qt::AlignTop, m_Item->Name().c_str());

    if (width < 40)
        return;

    QPixmap thumbnail = m_Item->GetMedia()->Thumbnail();
    QRectF thumbRect(10, 16, std::min(72, option->rect.width() - 10), 36);

    QSizeF size = thumbnail.size();
    size.scale(thumbRect.size(), Qt::KeepAspectRatio);

    QRectF drawRect(thumbRect.x(), thumbRect.y(), size.width(), size.height());
    drawRect.moveCenter(thumbRect.center());

    painter->drawPixmap(drawRect, thumbnail, thumbnail.rect());
}

void SPreviewTrackItem::Update()
{
    prepareGeometryChange();
    CalculateBoundingBox();
    setPos(m_Context->Geometry()->FrameToSceneX(m_Item->TimelineIn()), 0);

    update();
}

void SPreviewTrackItem::UpdatePosition(int x, int y)
{
    setPos(x, y);
    update();
}

void SPreviewTrackItem::CalculateBoundingBox()
{
    const double width = m_Context->Geometry()->FrameToSceneX(m_Item->TimelineOut() + 1) - m_Context->Geometry()->FrameToSceneX(m_Item->TimelineIn());
    m_BoundingRect = QRectF(0, 0, width, Sequencer::TrackItemHeight - 4);
}

bool SPreviewTrackItem::IsObstructed() const
{
    QPointF position = pos();
    if (STrack* track = m_Context->Controller()->TrackAt(position))
    {
        if (const SharedPlaybackTrack ptrack = track->Track())
        {
            v_frame_t start = m_Context->Geometry()->SceneXToFrame(position.x());
            return (bool)ptrack->ItemInRange(start, start + m_Item->Duration() - 1);
        }
    }

    return false;
}

VOID_NAMESPACE_CLOSE
