// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Context.h"

VOID_NAMESPACE_OPEN

namespace Sequence {

Context Context::Get(const Effect* effect)
{
    if (effect->GetEffectType() == Effect::EffectType::TRACK)
    {
        PlaybackTrack* track = effect->Track();
        return Context(
            track->Project(),
            track->Sequence()->Index(),
            track->Index(),
            track->EffectIndex(effect),
            effect->TimelineIn()
        );
    }
    
    TrackItem* item = effect->TimelineItem();
    PlaybackTrack* track = item->Track();
    return Context(
        track->Project(),
        track->Sequence()->Index(),
        track->Index(),
        item->EffectIndex(effect),
        effect->TimelineIn()
    );
}

Context Context::Get(const SharedTrackItem& item)
{
    const PlaybackTrack* track = item->Track();
    return Context(
        item->Project(),
        track->Sequence()->Index(),
        track->Index(),
        track->Type(),
        item->Index(),
        item->TimelineIn()
    );
}

Context Context::Get(const SharedPlaybackTrack& track, v_frame_t frame)
{
    return Context(track->Project(), track->Sequence()->Index(), track->Index(), track->Type(), frame);
}

Context Context::Get(const SharedPlaybackSequence& sequence)
{
    return Context(sequence->Project(), sequence->Index());
}

ResolvedContext Context::Resolve() const
{
    ResolvedContext rctx;
    rctx.type = type;
    rctx.project = project;
    rctx.sequence = project->Sequence(sequenceIdx);

    switch (type)
    {
        case Context::Type::TRACK:
            rctx.track = rctx.sequence->TrackAt(trackIdx, trackType);
            break;
        case Context::Type::TRACK_ITEM:
            rctx.track = rctx.sequence->TrackAt(trackIdx, trackType);
            rctx.trackItem = rctx.track->ItemAt(trackitemIdx);
            break;
        case Context::Type::TRACK_EFFECT:
            rctx.track = rctx.sequence->TrackAt(trackIdx, trackType);
            rctx.effect = rctx.track->EffectAt(effectIdx);
            break;
        case Context::Type::EFFECT:
            rctx.track = rctx.sequence->TrackAt(trackIdx, trackType);
            rctx.trackItem = rctx.track->ItemAt(trackitemIdx);
            rctx.effect = rctx.trackItem->EffectAt(effectIdx);
            break;
    }

    rctx.frame = frame;
    return rctx;
}

ResolvedContext Context::Resolve(const Type& type) const
{
    ResolvedContext rctx;
    rctx.type = type;
    rctx.project = project;
    rctx.sequence = project->Sequence(sequenceIdx);

    switch (type)
    {
        case Context::Type::TRACK:
            rctx.track = rctx.sequence->TrackAt(trackIdx, trackType);
            break;
        case Context::Type::TRACK_ITEM:
            rctx.track = rctx.sequence->TrackAt(trackIdx, trackType);
            rctx.trackItem = rctx.track->ItemAt(trackitemIdx);
            break;
        case Context::Type::TRACK_EFFECT:
            rctx.track = rctx.sequence->TrackAt(trackIdx, trackType);
            rctx.effect = rctx.track->EffectAt(effectIdx);
            break;
        case Context::Type::EFFECT:
            rctx.track = rctx.sequence->TrackAt(trackIdx, trackType);
            rctx.trackItem = rctx.track->ItemAt(trackitemIdx);
            rctx.effect = rctx.trackItem->EffectAt(effectIdx);
            break;
    }

    rctx.frame = frame;
    return rctx;
}

} // namespace Sequence

VOID_NAMESPACE_CLOSE
