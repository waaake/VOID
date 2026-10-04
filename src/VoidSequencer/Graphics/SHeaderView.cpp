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
#include "STrackHeader.h"
#include "Internal/Descriptors.h"
#include "VoidCore/Logging.h"
#include "VoidIconForge/IconForge.h"
#include "VoidSequencer/Widgets/SNameEditor.h"

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

void SHeaderView::EditTrackName()
{
    std::unordered_set<SharedPlaybackTrack> tracks = m_Context->SelectionModel()->SelectedTracks();
    if (tracks.empty())
        return;
    
    const SharedPlaybackTrack& track = *tracks.begin();
    if (STrackHeaderItem* header = m_Scene->Header(track))
    {
        const QRect r = header->NameRect();
        const QPoint m = mapFromScene(header->pos());
        m_NameEditor->setGeometry(r.x(), m.y() + r.height() * 0.5 - m_NameEditor->height() * 0.5, r.width(), 30);

        m_NameEditor->setVisible(true);
        m_NameEditor->setText(track->Name().c_str());
        m_NameEditor->setFocus();
    }
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

    /// Name Editor for Track Headers
    m_NameEditor = new SNameEditor(this);
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
    m_NameEditor->setVisible(false);
    connect(m_NameEditor, &SNameEditor::accepted, this, &SHeaderView::AcceptEdit);
    connect(m_NameEditor, &SNameEditor::cancelled, this, &SHeaderView::EditCancelled);
}

void SHeaderView::AcceptEdit(const QString& text)
{
    m_NameEditor->setVisible(false);
    const std::string& s = text.toStdString();
    for (const SharedPlaybackTrack& track : m_Context->SelectionModel()->SelectedTracks())
        track->SetName(s);
}

void SHeaderView::EditCancelled()
{
    m_NameEditor->setVisible(false);
}

VOID_NAMESPACE_CLOSE
