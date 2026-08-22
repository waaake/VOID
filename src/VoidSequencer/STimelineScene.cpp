// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <algorithm>

/* Qt */
#include <QPainter>
#include <QPalette>
#include <QGraphicsSceneMouseEvent>

/* Internal */
#include "STimelineScene.h"
#include "Graphics/SPlayheadItem.h"
#include "Graphics/SPreviewTrackItem.h"
#include "Graphics/SRazorItem.h"
#include "Graphics/STrack.h"
#include "Graphics/STrackItem.h"
#include "Graphics/STimelineEffect.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

// class TestItem : public QGraphicsRectItem
// {
// public:
//     TestItem()
//         : QGraphicsRectItem(0,0, 200, 200)
//     {
//         setBrush(Qt::red);
//     }
// };

STimelineScene::STimelineScene(SequencerContext* context, QObject* parent)
    : QGraphicsScene(parent)
    , m_Context(context)
    , m_Playhead(nullptr)
    , m_Razorhead(nullptr)
{
    // Can now be accessed directly by any other sub-component within the sequencer
    m_Context->Controller()->SetScene(this);

    setSceneRect(0, 0, Sequencer::SceneWidth, SceneHeight());
    connect(m_Context->Controller(), &SequencerController::frameChanged, this, &STimelineScene::UpdatePlayhead, Qt::DirectConnection);
}

void STimelineScene::AddTrack(const SharedPlaybackTrack& track)
{
    STrack* strack = new STrack(track, m_Context);
    m_Tracks.push_back(strack);
    addItem(strack);

    Update();
}

void STimelineScene::RemoveTrack(const SharedPlaybackTrack& track)
{
    auto _pred = [track](const STrack* t) -> bool { return track.get() == t->Track().get(); };
    auto it = std::find_if(m_Tracks.begin(), m_Tracks.end(), _pred);

    if (it == m_Tracks.end())
        return;

    STrack*& strack = *it;
    removeItem(strack);

    strack->deleteLater();
    delete strack;
    strack = nullptr;

    m_Tracks.erase(it);
}

void STimelineScene::Clear()
{
    for (auto& track : m_Tracks)
    {
        track->deleteLater();
        delete track;
        track = nullptr;
    }
    m_Tracks.clear();

    if (m_Playhead)
    {
        m_Playhead->deleteLater();
        delete m_Playhead;
        m_Playhead = nullptr;
    }

    if (m_Razorhead)
    {
        m_Razorhead->deleteLater();
        delete m_Razorhead;
        m_Razorhead = nullptr;
    }

    clear();
}

void STimelineScene::AddRazorhead()
{
    m_Razorhead = new SRazorItem(m_Context);
    addItem(m_Razorhead);
}

void STimelineScene::SetRazorX(int x)
{
    m_Razorhead->SetX(x);
}

void STimelineScene::ToggleRazorhead(bool visible)
{
    if (!m_Razorhead) AddRazorhead();
    m_Razorhead->setVisible(visible);
}

void STimelineScene::AddPlayhead()
{
    m_Playhead = new SPlayheadItem(m_Context);
    addItem(m_Playhead);
}

void STimelineScene::UpdatePlayhead()
{
    m_Playhead->Update();
}

void STimelineScene::Update()
{
    setSceneRect(0, 0, Sequencer::SceneWidth, SceneHeight());
}

void STimelineScene::UpdateItems()
{
    setSceneRect(0, 0, Sequencer::SceneWidth, SceneHeight());
    m_Playhead->Update();

    for (STrack* track : m_Tracks)
    {
        track->Update();
        track->UpdateItems();
        track->UpdateEffects();
    }
}

STrack* STimelineScene::TrackAt(int index) const
{
    return m_Tracks.at(index);
}

STrack*& STimelineScene::TrackAt(int index)
{
    return m_Tracks.at(index);
}

void STimelineScene::SelectItems(const QRectF& rect)
{
    const QList<QGraphicsItem*> hits = items(rect, Qt::IntersectsItemShape);
    if (hits.empty())
        return;

    std::vector<SharedTrackItem> trackitems;
    std::vector<Effect*> effects;
    trackitems.reserve(hits.size());
    effects.reserve(hits.size());
    for (auto& item : hits)
    {
        if (const auto& trackitem = dynamic_cast<STrackItem*>(item))
            trackitems.emplace_back(trackitem->TrackItem());
        else if (const auto& effect = dynamic_cast<STimelineEffect*>(item))
            effects.push_back(effect->TimelineEffect());
    }

    m_Context->SelectionModel()->Select(trackitems);
    m_Context->SelectionModel()->Select(effects);
}

void STimelineScene::InitDraggableItems(const std::vector<SharedMediaClip>& media)
{
    m_DraggedItems.reserve(media.size());

    int duration = 0;
    for (const SharedMediaClip& clip : media)
    {
        int offset = clip->FirstFrame() - duration;
        duration = duration + clip->Duration();
        SharedTrackItem item = std::make_shared<TrackItem>(
                                        clip,
                                        clip->FirstFrame() - offset,
                                        clip->LastFrame() - offset,
                                        offset
                                    );

        SPreviewTrackItem* preview = new SPreviewTrackItem(item, m_Context);
        m_DraggedItems.push_back(preview);
        addItem(preview);
    }

    VOID_LOG_INFO("Constructed {0} Previews", static_cast<int>(m_DraggedItems.size()));
}

void STimelineScene::MoveDraggableItems(const QPointF& position)
{
    if (m_DraggedItems.empty()) return;

    int x = m_DraggedItems[0]->pos().x();
    for (SPreviewTrackItem*& item : m_DraggedItems)
    {
        int offset = item->pos().x() - x;
        item->UpdatePosition(offset + position.x(), position.y());
    }
}

void STimelineScene::DestroyDraggableItems()
{
    for (SPreviewTrackItem*& item : m_DraggedItems)
    {
        removeItem(item);
        item->deleteLater();
        delete item;
        item = nullptr;
    }

    VOID_LOG_INFO("Destroyed {0} Previews", static_cast<int>(m_DraggedItems.size()));
    m_DraggedItems.clear();
}

void STimelineScene::DropItems(const QPointF& position)
{
    STrack* track = m_Context->Controller()->TrackAt(position);
    if (track)
    {
        std::vector<std::pair<const SharedMediaClip, v_frame_t>> media;
        media.reserve(m_DraggedItems.size());

        int offset = 0;
        v_frame_t frame = m_Context->Geometry()->SceneXToFrame(position.x());

        for (SPreviewTrackItem*& item : m_DraggedItems)
        {
            media.emplace_back(
                item->TrackItem()->GetMedia(),
                frame + offset
            );
            offset += item->TrackItem()->Duration();

            removeItem(item);
            item->deleteLater();
            delete item;
            item = nullptr;
        }

        m_DraggedItems.clear();
        m_Context->Controller()->CreateTrackItems(media, track->Track());
    }
    else
    {
        DestroyDraggableItems();
    }
}

void STimelineScene::drawBackground(QPainter* painter, const QRectF& rect)
{
    painter->fillRect(rect, palette().color(QPalette::Base));
}

void STimelineScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if ((event->button() == Qt::LeftButton) && !(event->modifiers() & Qt::ControlModifier))
        m_Context->SelectionModel()->Clear();

    QGraphicsScene::mousePressEvent(event);
}

int STimelineScene::SceneHeight() const
{
    return Sequencer::RulerHeight + m_Tracks.size() * (Sequencer::TrackHeight + Sequencer::TrackSpacing);
}

VOID_NAMESPACE_CLOSE
