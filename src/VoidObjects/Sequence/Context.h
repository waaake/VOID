// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCE_CONTEXT_H
#define _SEQUENCE_CONTEXT_H

/* STD */
#include <memory>

/* Internal */
#include "Definition.h"
#include "Internal/Descriptors.h"
#include "VoidObjects/Project/Project.h"
#include "VoidObjects/Sequence/Sequence.h"
#include "VoidObjects/Sequence/Track.h"
#include "VoidObjects/Sequence/TrackItem.h"
#include "VoidObjects/Effects/Effects.h"

VOID_NAMESPACE_OPEN

namespace Sequence {

struct ResolvedContext;

/**
 * @brief A Sequence context is essentially a basic serialized version of
 * any entity within the sequence.
 * Example could be a TrackItem, fow which the context holds information about it's current track's index
 * it's current sequence and it's index, pointer to the current project and track item's index in the current track.
 *
 */
struct VOID_API Context
{
    enum class Type { SEQUENCE, TRACK, TRACK_ITEM, TRACK_EFFECT, EFFECT };

    Type type;
    Core::Project* project;
    int sequenceIdx;
    int trackIdx { -1 };
    int trackitemIdx { -1 };
    int effectIdx { -1 };
    v_frame_t frame { 0 };
    TrackType trackType { TrackType::VIDEO };
    Context()
        : type(Type::SEQUENCE), project(nullptr), sequenceIdx(-1) {}
    Context(Core::Project* p, int seq)
        : type(Type::SEQUENCE), project(p), sequenceIdx(seq) {}
    Context(Core::Project* p, int seq, int track, const TrackType& ty, v_frame_t frame = 0)
        : type(Type::TRACK), project(p), sequenceIdx(seq), trackIdx(track), trackType(ty), frame(frame) {}
    Context(Core::Project* p, int seq, int track, const TrackType& ty, int trackitem, v_frame_t frame = 0)
        : type(Type::TRACK_ITEM), project(p), sequenceIdx(seq), trackIdx(track), trackitemIdx(trackitem), frame(frame), trackType(ty) {}
    Context(Core::Project* p, int seq, int track, int effect, v_frame_t frame = 0)
        : type(Type::TRACK_EFFECT), project(p), sequenceIdx(seq), trackIdx(track), effectIdx(effect), frame(frame), trackType(TrackType::VIDEO) {}
    Context(Core::Project* p, int seq, int track, const TrackType& ty, int trackitem, int effect, v_frame_t frame = 0)
        : type(Type::EFFECT), project(p), sequenceIdx(seq), trackIdx(track), trackitemIdx(trackitem), effectIdx(effect), frame(frame), trackType(ty) {}
    ResolvedContext Resolve() const;
    ResolvedContext Resolve(const Type& type) const;

    static Context Get(const Effect* effect);
    static Context Get(const SharedTrackItem& item);
    static Context Get(const SharedPlaybackTrack& track, v_frame_t frame = 0);
    static Context Get(const SharedPlaybackSequence& sequence);
};

/**
 * @brief Struct holding deserialized entities from the EntityContext to describe the entity
 * in real-time.
 * This is effectively the actual item's address within the project.
 * 
 */
struct VOID_API ResolvedContext
{
    Core::Project* project;
    SharedPlaybackSequence sequence { nullptr };
    SharedPlaybackTrack track { nullptr };
    SharedTrackItem trackItem { nullptr };
    Effect* effect { nullptr };
    v_frame_t frame;
    TrackType tracktype;
    Context::Type type;
};

} // namespace Sequence

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCE_CONTEXT_H
