// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>
#include <QScrollBar>

/* Internal */
#include "STrackHeaderWidget.h"
#include "STrackHeader.h"
#include "VoidQExtensions/Frame.h"
#include "VoidUi/Sequencer/SDescriptors.h"
#include "VoidUi/Sequencer/SContext.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

STrackHeaderWidget::STrackHeaderWidget(SequencerContext* context, QWidget* parent)
    : QWidget(parent)
    , m_Context(context)
{
    setFixedWidth(Sequencer::TrackHeaderWidth);
    Build();
}

void STrackHeaderWidget::AddTrack(const SharedPlaybackTrack& track)
{
    STrackHeader* header = new STrackHeader(track, m_Context, this);
    header->setObjectName(track->Vuid().c_str());
    m_ScrollLayout->addWidget(header);
}

void STrackHeaderWidget::RemoveTrack(const SharedPlaybackTrack& track)
{
    if (auto header = findChild<STrackHeader*>(track->Vuid().c_str()))
    {
        header->setParent(nullptr);
        header->setVisible(false);
        header->deleteLater();
        delete header;
        header = nullptr;
    }
}

void STrackHeaderWidget::Clear()
{
    for (auto& header : findChildren<STrackHeader*>())
    {
        header->setParent(nullptr);
        header->setVisible(false);
        header->deleteLater();
        delete header;
        header = nullptr;
    }
}

void STrackHeaderWidget::Update()
{
    for (auto& header : findChildren<STrackHeader*>())
        header->Update();
    
    m_ScrollLayout->update();
}

void STrackHeaderWidget::SetScroll(int value)
{
    m_ScrollArea->verticalScrollBar()->setValue(value);
}

void STrackHeaderWidget::Build()
{
    m_Layout = new QVBoxLayout(this);

    BaseWidget* content = new BaseWidget;
    content->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

    m_ScrollLayout = new QVBoxLayout(content);
    m_ScrollLayout->setAlignment(Qt::AlignTop);
    m_ScrollLayout->setContentsMargins(0, 0, 0, 0);
    m_ScrollLayout->setSpacing(Sequencer::TrackSpacing);

    m_ScrollLayout->addItem(new QSpacerItem(0, Sequencer::RulerHeight, QSizePolicy::Expanding, QSizePolicy::Fixed));

    m_ScrollArea = new QScrollArea;
    m_ScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_ScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_ScrollArea->setWidgetResizable(true);
    m_ScrollArea->setWidget(content);
    m_ScrollArea->setFrameShape(QFrame::NoFrame);

    m_Layout->addWidget(m_ScrollArea);
    m_Layout->setContentsMargins(0, 0, 0, 0);
}

VOID_NAMESPACE_CLOSE
