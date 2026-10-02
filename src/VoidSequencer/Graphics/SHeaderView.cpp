// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QMimeData>
#include <QMouseEvent>
#include <QPainter>
#include <QStyle>
#include <QScrollBar>

/* Internal */
#include "SContext.h"
#include "SHeaderView.h"
#include "SHeaderScene.h"
#include "Internal/Descriptors.h"
#include "VoidCore/Logging.h"
#include "VoidIconForge/IconForge.h"

VOID_NAMESPACE_OPEN

SHeaderView::SHeaderView(SequencerContext* context, QWidget* parent)
    : QGraphicsView(parent)
    , m_Context(context)
{
    Build();
    Setup();
}

SHeaderView::~SHeaderView()
{
    m_Scene->deleteLater();
    delete m_Scene;
    m_Scene = nullptr;
}

void SHeaderView::AddVideoTrack(const SharedPlaybackTrack& track)
{
    m_Scene->AddVideoTrack(track);
}

void SHeaderView::AddAudioTrack(const SharedPlaybackTrack& track)
{
    m_Scene->AddAudioTrack(track);
}

void SHeaderView::RemoveTrack(const SharedPlaybackTrack& track)
{
    m_Scene->RemoveTrack(track);
}

void SHeaderView::Refresh()
{
    m_Scene->UpdateItems();
}

void SHeaderView::Clear()
{
    m_Scene->Clear();
}

void SHeaderView::ResetScroll()
{
    centerOn(0, 0);
}

void SHeaderView::SetScroll(int value)
{
    verticalScrollBar()->setValue(value);
}

void SHeaderView::Build()
{
    m_Scene = new SHeaderScene(m_Context, this);
    setScene(m_Scene);
    /// TODO: check why do we need to set this explicitly
    /// Without this, the view is always at the center of the width (even height)
    centerOn(0, 0);
}

void SHeaderView::Setup()
{
    setRenderHint(QPainter::Antialiasing, true);
    setRenderHint(QPainter::TextAntialiasing, true);

    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    setDragMode(QGraphicsView::NoDrag);

    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setCacheMode(QGraphicsView::CacheBackground);

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // setAcceptDrops(true);
}

VOID_NAMESPACE_CLOSE
