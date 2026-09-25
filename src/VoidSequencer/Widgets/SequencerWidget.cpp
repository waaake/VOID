// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QColorDialog>
#include <QCursor>
#include <QScrollBar>
#include <QStyle>
#include <QWheelEvent>

/* Internal */
#include "SequencerWidget.h"
#include "VoidDocker/DockPanel.h"
#include "VoidObjects/Sequence/Context.h"

VOID_NAMESPACE_OPEN

SequencerWidget::SequencerWidget(TimelineController* controller, QWidget* parent)
    : QWidget(parent)
{
    m_Context.Controller()->SetTimeController(controller);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setFocusPolicy(Qt::WheelFocus);

    Build();
}

SequencerWidget::~SequencerWidget()
{
    m_Toolbar->deleteLater();
    delete m_Toolbar;
    m_Toolbar = nullptr;

    m_TrackHeader->deleteLater();
    delete m_TrackHeader;
    m_TrackHeader = nullptr;

    m_View->deleteLater();
    delete m_View;
    m_View = nullptr;

    m_Ruler->deleteLater();
    delete m_Ruler;
    m_Ruler = nullptr;

    m_VersionSwitcher->deleteLater();
    delete m_VersionSwitcher;
    m_VersionSwitcher = nullptr;
}

void SequencerWidget::ResetTabText()
{
    if (DockPanel* panel = dynamic_cast<DockPanel*>(parent()))
        panel->SetTabText(m_Context.HasActiveSequence() ? m_Context.Sequence()->Name().c_str() : "Sequencer");
}

void SequencerWidget::wheelEvent(QWheelEvent* event)
{
    QWidget::wheelEvent(event);
    if (event->modifiers() & Qt::ControlModifier)
    {
        const int value = m_HZoomSlider->value();
        if (event->angleDelta().y() > 0)
            m_HZoomSlider->setValue(value + (value > 40 ? 3 : 1));
        else
            m_HZoomSlider->setValue(value - (value > 40 ? 3 : 1));
    }
}

void SequencerWidget::Clear()
{
    m_TrackHeader->Clear();
    m_View->Clear();
    m_View->AddPlayhead();
}

void SequencerWidget::Build()
{
    m_Menu = new SequencerContextMenu(&m_Context, this);

    m_Layout = new QHBoxLayout(this);

    QGridLayout* grid = new QGridLayout();
    grid->setSpacing(0);
    grid->setContentsMargins(0, 0, 0, 0);

    m_Toolbar = new SToolbar;
    m_TrackHeader = new STrackHeaderWidget(&m_Context);

    m_HZoomSlider = new QSlider(Qt::Horizontal, this);
    m_HZoomSlider->setFixedHeight(style()->pixelMetric(QStyle::PM_ScrollBarExtent) + 2);
    m_HZoomSlider->setMinimum(1);
    m_HZoomSlider->setMaximum(200);
    m_HZoomSlider->setValue(m_Context.Geometry()->PixelsPerFrame() * 10);

    m_View = new STimelineView(&m_Context);
    m_Ruler = new STimelineRuler(m_View, &m_Context);

    m_VersionSwitcher = new SVersionSwitcher(&m_Context, this);

    grid->addWidget(m_Ruler, 0, 1);

    grid->addWidget(m_TrackHeader, 1, 0);
    grid->addWidget(m_HZoomSlider, 2, 0);
    grid->addWidget(m_View, 1, 1, 2, 1);

    grid->setColumnMinimumWidth(0, Sequencer::TrackHeaderWidth);
    grid->setRowMinimumHeight(0, Sequencer::RulerHeight);

    grid->setColumnStretch(1, 1);
    grid->setRowStretch(1, 1);

    m_Layout->setSpacing(0);
    m_Layout->setContentsMargins(0, 0, 0, 0);

    m_Layout->addWidget(m_Toolbar);
    m_Layout->addLayout(grid);
}

VOID_NAMESPACE_CLOSE
