// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

/* Internal */
#include "STimelineEffect.h"
#include "VoidObjects/Effects/Effects.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

STimelineEffect::STimelineEffect(Effect* effect, SequencerContext* context, QGraphicsItem* parent)
    : STimelineItem(context, parent)
    , m_Effect(effect)
{
    CalculateBoundingBox();
    connect(m_Context->SelectionModel(), &SSelectionModel::selectionChanged, this, [this]() { update(); });
    connect(m_Context->SelectionModel(), &SSelectionModel::effectSelectionChanged, this, [this]() { update(); });
    connect(m_Effect, &Effect::rangeChanged, this, &STimelineEffect::Update);
    connect(m_Effect, &Effect::updated, this, [this]() { update(); });
}

void STimelineEffect::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{

    if (m_Context->SelectionModel()->IsSelected(m_Effect->TimelineItem()) || m_Context->SelectionModel()->IsSelected(m_Effect))
    {
        painter->setPen(QPen(option->palette.color(QPalette::Highlight)));
        painter->setBrush(option->palette.color(QPalette::Highlight).darker(180));
    }
    else
    {
        painter->setPen(QPen(option->palette.color(QPalette::Dark)));
        painter->setBrush(
            m_Effect->Enabled() ? m_Effect->Color() : m_Effect->Color().darker(350)
        );
    }

    painter->drawRect(boundingRect());
    painter->fillRect(
        2,
        2,
        std::min(6, static_cast<int>(boundingRect().width()) - 2),
        Sequencer::TimelineEffectHeight - 4,
        m_Effect->Enabled() ? m_Effect->Color() : m_Effect->Color().darker(350)
    );
    painter->setPen(Qt::black);
    painter->drawText(boundingRect().adjusted(10, 0, -2, 0), Qt::AlignLeft | Qt::AlignVCenter, m_Effect->Name().c_str());
}

void STimelineEffect::SetWidth(double width)
{
    prepareGeometryChange();
    m_BoundingRect = QRectF(0, 0, width, Sequencer::TimelineEffectHeight);
    update();
}

void STimelineEffect::Update()
{
    prepareGeometryChange();
    CalculateBoundingBox();

    update();
}

void STimelineEffect::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (m_Context->Action() == SequencerAction::NONE)
        {
            if (event->modifiers() & Qt::ControlModifier)
                m_Context->SelectionModel()->Toggle(m_Effect);
            else
                m_Context->SelectionModel()->Select(m_Effect);
        }
    }

    STimelineItem::mousePressEvent(event);
}

void STimelineEffect::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    m_Context->Controller()->EditEffect(m_Effect);
    STimelineItem::mouseDoubleClickEvent(event);
}

void STimelineEffect::CalculateBoundingBox()
{
    const double width = m_Context->Geometry()->FrameToSceneX(m_Effect->TimelineOut() + 1) - m_Context->Geometry()->FrameToSceneX(m_Effect->TimelineIn());
    m_BoundingRect = QRectF(0, 0, width, Sequencer::TimelineEffectHeight);
}

VOID_NAMESPACE_CLOSE
