// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>
#include <QScrollBar>
#include <QResizeEvent>

/* Internal */
#include "STimelineRuler.h"
#include "VoidCore/Logging.h"
#include "VoidSequencer/SDescriptors.h"
#include "VoidSequencer/STimelineView.h"
#include "VoidSequencer/SContext.h"

VOID_NAMESPACE_OPEN

STimelineRuler::STimelineRuler(STimelineView* view, SequencerContext* context, QWidget* parent)
    : QWidget(parent)
    , m_View(view)
    , m_Context(context)
{
    setFixedHeight(Sequencer::RulerHeight);
    connect(m_View->horizontalScrollBar(), &QScrollBar::valueChanged, this, static_cast<void (STimelineRuler::*)(void)>(&STimelineRuler::update), Qt::DirectConnection);
    connect(m_Context->Controller(), &SequencerController::frameChanged, this, static_cast<void (STimelineRuler::*)(void)>(&STimelineRuler::update), Qt::DirectConnection);
}

void STimelineRuler::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), palette().color(QPalette::AlternateBase));

    painter.setPen(palette().color(QPalette::Text));
    MFrameRange visible = m_View->VisibleRange();

    float uwidth = (float)width() / visible.duration;
    for (int frame = visible.startframe; frame <= visible.endframe; ++frame)
    {
        if (frame % Sequencer::RulerMajorTickInterval == 0)
        {
            float x = (frame - visible.startframe) * uwidth;
            painter.drawLine(x, height() - 10, x, height() - 2);
        }
        else if (frame % Sequencer::RulerMinorTickInterval == 0)
        {
            float x = (frame - visible.startframe) * uwidth;
            painter.drawLine(x, height() - 6, x, height() - 2);
        }
    }

    // Current Frame
    if (visible.Contains(m_Context->Controller()->CurrentFrame()))
    {
        painter.setPen(palette().color(QPalette::Highlight));

        float x = (m_Context->Controller()->CurrentFrame() - visible.startframe) * uwidth;
        painter.drawLine(x, 0, x, height());

        QPolygonF triangle;
        triangle << QPointF(x, Sequencer::PlayheadTriangleHeight);
        triangle << QPointF(x - Sequencer::PlayheadTriangleWidth / 2.f, 0);
        triangle << QPointF(x + Sequencer::PlayheadTriangleWidth / 2.f, 0);
    
        painter.setBrush(palette().color(QPalette::Highlight));
        painter.drawPolygon(triangle);
    }
}

void STimelineRuler::mousePressEvent(QMouseEvent* event)
{
    QWidget::mousePressEvent(event);
    int x = event->pos().x();

    MFrameRange visible = m_View->VisibleRange();
    m_Pressed = true;
    m_Context->Controller()->RequestFrameChange(visible.startframe + m_Context->Geometry()->SceneXToFrame(x));
}

void STimelineRuler::mouseMoveEvent(QMouseEvent* event)
{
    QWidget::mouseMoveEvent(event);
    if (m_Pressed)
    {
        MFrameRange visible = m_View->VisibleRange();
        int x = event->pos().x();

        m_Context->Controller()->RequestFrameChange(visible.startframe + m_Context->Geometry()->SceneXToFrame(x));
    }
}

void STimelineRuler::mouseReleaseEvent(QMouseEvent* event)
{
    QWidget::mouseReleaseEvent(event);
    m_Pressed = false;
}

VOID_NAMESPACE_CLOSE
