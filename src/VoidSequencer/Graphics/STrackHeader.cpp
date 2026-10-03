// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

/* Internal */
#include "STrackHeader.h"
#include "VoidIconForge/IconForge.h"
#include "VoidSequencer/SContext.h"
#include "VoidSequencer/STimelineGeometry.h"

VOID_NAMESPACE_OPEN

constexpr int margin = 8;
constexpr int iconSize = 18;
constexpr int spacing = 10;
constexpr int nameWidth = 90;

// https://doc.qt.io/qt-6/qgraphicsproxywidget.html -- we need the lineedit over the label when editing the name of the track

STrackHeaderItem::STrackHeaderItem(const SharedPlaybackTrack& track, SequencerContext* context, QGraphicsItem* parent)
    : STimelineItem(context, parent)
    , m_Track(track)
{
    setAcceptHoverEvents(true);

    setZValue(Sequencer::ZTrack);
    Connect();

    int index = track->Index();
    setPos(0, context->Geometry()->TrackTop(index, track->Type()));
    Resize(index);
}

void STrackHeaderItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    const QPalette& p = option->palette;
    painter->setPen(Qt::black);
    painter->setBrush(Background(p));
    painter->drawRect(boundingRect().adjusted(1, 0, -1, 0));

    painter->setPen(p.color(QPalette::Text));
    painter->drawText(NameRect(), Qt::AlignVCenter | Qt::AlignRight, m_Track->Name().c_str());

    painter->drawPixmap(
        StateRect().topLeft(),
        m_Track->Enabled()
            ? IconForge::GetPixmap(IconType::icon_visible, p.color(QPalette::Text).darker(100), 14)
            : IconForge::GetPixmap(IconType::icon_visible_off, p.color(QPalette::Highlight).darker(100), 14)
    );

    painter->drawPixmap(
        LockRect().topLeft(),
        m_Track->Locked()
            ? IconForge::GetPixmap(IconType::icon_lock, p.color(QPalette::Highlight).darker(100), 14)
            : IconForge::GetPixmap(IconType::icon_lock_open, p.color(QPalette::Text).darker(100), 14)
    );
}

void STrackHeaderItem::Update()
{
    prepareGeometryChange();
    int index = m_Track->Index();
    setPos(0, m_Context->Geometry()->TrackRect(index).top());
    Resize(index);

    update();
}

void STrackHeaderItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (LockRect().contains(event->pos().toPoint()))
        {
            m_Context->Controller()->ToggleTrackLock(m_Track);
        }
        else if (StateRect().contains(event->pos().toPoint()))
        {
            m_Context->Controller()->ToggleTrackState(m_Track);
        }
        else if (event->modifiers() & Qt::ControlModifier)
        {
            m_Context->SelectionModel()->Toggle(m_Track);
        }
        else
        {
            m_Context->SelectionModel()->Clear();
            m_Context->SelectionModel()->Select(m_Track);
        }
    }

    STimelineItem::mousePressEvent(event);
}

void STrackHeaderItem::Connect()
{
    auto* ptr = m_Track.get();
    connect(ptr, &PlaybackTrack::maxEffectsChanged, this, &STrackHeaderItem::Update);
    connect(ptr, &PlaybackTrack::updated, this, [this]() -> void { update(); });
    connect(ptr, &PlaybackTrack::stateChanged, this, [this]() -> void { update(); });
    connect(m_Context->SelectionModel(), &SSelectionModel::trackSelectionChanged, this, [this]() { update(); });
}

void STrackHeaderItem::Resize(int index)
{
    m_BoundingRect = QRectF(
        0,
        0,
        Sequencer::TrackHeaderWidth,
        m_Track->Type() == Sequence::Type::VIDEO
            ? m_Context->Geometry()->VideoTrackHeight(index)
            : m_Context->Geometry()->AudioTrackHeight(index)
    );

    m_LockRect = QRect(margin, (m_BoundingRect.height() - iconSize) * 0.5, iconSize, iconSize);
    m_StateRect = QRect(m_LockRect.right() + spacing, (m_BoundingRect.height() - iconSize) * 0.5, iconSize, iconSize);
    m_NameRect = QRect(m_StateRect.right() + spacing, 0, m_BoundingRect.width() - (m_StateRect.right() + spacing + margin), m_BoundingRect.height());
}

QColor STrackHeaderItem::Background(const QPalette& palette) const
{
    if (m_Context->SelectionModel()->IsSelected(m_Track))
        return palette.color(QPalette::Highlight).darker(150);
    
    return m_Track->Type() == Sequence::Type::VIDEO ? palette.color(QPalette::Dark) : QColor(45, 55, 45);
}

VOID_NAMESPACE_CLOSE
