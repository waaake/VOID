// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>
#include <QScrollBar>
#include <QResizeEvent>

/* Internal */
#include "STimelineRuler.h"
#include "VoidCore/Logging.h"
#include "VoidSequencer/Descriptors.h"
#include "VoidSequencer/SContext.h"
#include "VoidSequencer/Graphics/STimelineView.h"

VOID_NAMESPACE_OPEN

STimelineRuler::STimelineRuler(STimelineView* view, SequencerContext* context, QWidget* parent)
    : QWidget(parent)
    , m_Timekeeper(Timekeeper::Instance())
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
    painter.fillRect(rect(), palette().color(QPalette::Dark).lighter(120));

    painter.setPen(QPen(Qt::gray, 1));
    MFrameRange visible = m_View->VisibleRange();

    const int frameInterval = TimeDisplayInterval(m_Context->Geometry()->PixelsPerFrame());
    const float uwidth = (float)width() / visible.duration;
    for (int frame = visible.startframe; frame <= visible.endframe; ++frame)
    {
        if (frame % frameInterval == 0)
        {
            const int offset = (frame == visible.startframe ? 0 : 100);
            const float x = (frame - visible.startframe) * uwidth;
            const QFlags<Qt::AlignmentFlag> f = (frame == visible.startframe ? Qt::AlignTop | Qt::AlignLeft : Qt::AlignTop | Qt::AlignHCenter);

            QRect r(x - offset, 0, 200, height());

            painter.drawText(r, f, m_Timekeeper.DisplayFrame(frame).c_str());
            painter.drawLine(x, height() - 14, x, height() - 2);
        }
        else if (frame % Sequencer::RulerMajorTickInterval == 0)
        {
            const float x = (frame - visible.startframe) * uwidth;
            painter.drawLine(x, height() - 10, x, height() - 2);
        }
        else if (frame % Sequencer::RulerMinorTickInterval == 0)
        {
            const float x = (frame - visible.startframe) * uwidth;
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

int STimelineRuler::TimeDisplayInterval(const double pixelsPerFrame) const
{
    if (pixelsPerFrame > 8.0)
        return 50;
    else if (pixelsPerFrame > 4.0)
        return 100;
    else if (pixelsPerFrame > 2.0)
        return 200;
    else if (pixelsPerFrame > 1.0)
        return 400;
    else if (pixelsPerFrame > 0.25)
        return 800;
    else if (pixelsPerFrame > 0.15)
        return 1600;
    else
        return 3200;
}

VOID_NAMESPACE_CLOSE
