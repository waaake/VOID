// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>

/* Internal */
#include "STrackHeaderWidget.h"
#include "VoidUi/Sequencer/SDescriptors.h"
#include "VoidUi/Sequencer/STimelineGeometry.h"

VOID_NAMESPACE_OPEN

STrackHeaderWidget::STrackHeaderWidget(STimelineGeometry* geometry, QWidget* parent)
    : QWidget(parent)
{
    setFixedWidth(Sequencer::TrackHeaderWidth);
}

void STrackHeaderWidget::SetSequence(const SharedPlaybackSequence& sequence)
{
    m_Sequence = sequence;
    update();
}

void STrackHeaderWidget::Clear()
{
    m_Sequence = nullptr;
    update();
}

void STrackHeaderWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    if (!m_Sequence)
        return;

    for (int i = 0; i < m_Sequence->NumVideoTracks(); ++i)
    {
        painter.fillRect(m_Geometry->TrackHeaderRect(i), palette().color(QPalette::Dark));

        painter.setPen(palette().color(QPalette::Text));
        painter.drawText(
            m_Geometry->TrackHeaderRect(i).adjusted(8, 0, -8, 0),
            Qt::AlignVCenter, m_Sequence->VideoTrackAt(i)->Name().c_str()
        );
    }
}

VOID_NAMESPACE_CLOSE
