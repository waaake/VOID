// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>

/* Internal */
#include "SContext.h"
#include "STimelineView.h"
#include "STimelineScene.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

STimelineView::STimelineView(SequencerContext* context, QWidget* parent)
    : QGraphicsView(parent)
    , m_Context(context)
{
    Build();
    Setup();
}

void STimelineView::SetSequence(const SharedPlaybackSequence& sequence)
{
    m_Scene->SetSequence(sequence);
    m_Scene->AddPlayhead();
}

void STimelineView::Clear()
{
    m_Scene->Clear();
}

void STimelineView::Focus()
{
    const auto& selected = m_Scene->selectedItems();
    selected.empty() ? centerOn(0, 0) : centerOn(selected[0]);
}

void STimelineView::Build()
{
    m_Scene = new STimelineScene(m_Context, this);
    setScene(m_Scene);

    m_Scene->AddPlayhead();
}

void STimelineView::Setup()
{
    setRenderHint(QPainter::Antialiasing, true);
    setRenderHint(QPainter::TextAntialiasing, true);

    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    setDragMode(QGraphicsView::NoDrag);
    setFrameShape(QFrame::NoFrame);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    setAlignment(Qt::AlignLeft | Qt::AlignTop);

    setCacheMode(QGraphicsView::CacheBackground);
}

VOID_NAMESPACE_CLOSE
