// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "SController.h"
#include "STimelineScene.h"
#include "VoidCore/Logging.h"
#include "Graphics/STrack.h"
// #include "VoidObjects/Sequence/Track.h"
#include "VoidUi/Media/MediaBridge.h"
#include "VoidUi/Commands/SequenceCommands.h"

VOID_NAMESPACE_OPEN

void SequencerController::SetScene(QGraphicsScene* scene)
{
    m_Scene = scene;
}

void SequencerController::MoveItem(SharedTrackItem& item, v_frame_t frame)
{
    // Item was dragged and returned back to the same position
    if (item->TimelineIn() == frame)
        return;

    _MediaBridge.PushCommand(new MoveTrackItemCommand(item, frame));

    // PlaybackTrack* track = item->Track();
    // VOID_LOG_INFO("Track is nullptr: {0}", track == nullptr);

    // if (track)
    //     return track->MoveItem(item, frame);

    // return false;
}

// void SequencerController::MoveItem(SharedPlaybackTrack& track, SharedTrackItem& item, v_frame_t frame)
// {
//     PlaybackTrack* otrack = item->Track();
//     if (otrack)
//     {
//         // The item was successfully added to the new track and now can be removed from the previous one
//         // This is required for the time being where we are not adding any items over existing items
//         // Later this check might not be needed, if we're straight over-writing on existing items
//         if (track->AddItem(item, frame))
//             otrack->RemoveItem(item);
//     }
// }

void SequencerController::MoveItem(SharedTrackItem& item, int currentTrackIndex, int trackIndex, v_frame_t frame)
{
    // if (currentTrackIndex == trackIndex)
    //     return;

    _MediaBridge.PushCommand(new MoveItemToTrackCommand(this, item, currentTrackIndex, trackIndex, frame));
}

STrack* SequencerController::TrackAt(const QPointF& position) const
{
    // return m_Scene ? qgraphicsitem_cast<STrack*>(m_Scene->itemAt(position, QTransform())) : nullptr;
    if (m_Scene)
    {
        QList<QGraphicsItem*> items = m_Scene->items(position, Qt::IntersectsItemShape, Qt::AscendingOrder);
        for (QGraphicsItem*& item : items)
        {
            if (STrack* track = dynamic_cast<STrack*>(item))
                return track;
        }

    }
    return nullptr;
}

STrack* SequencerController::TrackAt(int index) const
{
    if (STimelineScene* scene = dynamic_cast<STimelineScene*>(m_Scene))
        return scene->TrackAt(index);
    return nullptr;
}

STrack* SequencerController::TrackAt(int index)
{
    if (STimelineScene* scene = dynamic_cast<STimelineScene*>(m_Scene))
        return scene->TrackAt(index);
    return nullptr;
}

VOID_NAMESPACE_CLOSE
