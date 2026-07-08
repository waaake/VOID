// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>

/* Internal */
#include "STimelineView.h"
#include "STimelineScene.h"
#include "STimelineGeometry.h"

VOID_NAMESPACE_OPEN

STimelineView::STimelineView(STimelineGeometry* geometry, QWidget* parent)
    : QGraphicsView(parent)
    , m_Geometry(geometry)
{
    Build();
    Setup();
}

void STimelineView::SetSequence(const SharedPlaybackSequence& sequence)
{
    m_Scene->SetSequence(sequence);
}

void STimelineView::Clear()
{
    m_Scene->Clear();
}

void STimelineView::Build()
{
    m_Scene = new STimelineScene(m_Geometry, this);
    setScene(m_Scene);
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
