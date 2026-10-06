// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_TAG_WIDGET_H
#define _SEQUENCER_TAG_WIDGET_H

/* Internal */
#include "Definition.h"
#include "VoidObjects/Media/MediaClip.h"
#include "VoidQExtensions/Dialog.h"
#include "VoidToolbox/Editor/TagEditor.h"
#include "VoidSequencer/SContext.h"

VOID_NAMESPACE_OPEN

/// Sequence Tags

class EntityTagWidget : public TagWidget
{
    enum class Type
    {
        TRACK,
        TRACK_ITEM,
        SEQUENCE
    };
public:
    EntityTagWidget(SequencerContext* context, QWidget* parent = nullptr);
    ~EntityTagWidget();

    // void Set(const SharedTrackItem& item);
    void Set(const SharedPlaybackTrack& track);
    // void Set(const SharedPlaybackSequence& sequence);

private: /* Members */
    VoidObject* m_Entity;
    TagMetadataModel* m_Metadata;
    SequencerContext* m_Context;

    Type m_Type;

private: /* Methods */
    void AddTag();
};

class EntityTagEditor : public TagEditor
{
    enum class Type
    {
        TRACK,
        TRACK_ITEM,
        SEQUENCE
    };
public:
    EntityTagEditor(SequencerContext* context, QWidget* parent = nullptr);

    // void Set(const SharedTrackItem& item);
    void Set(const SharedPlaybackTrack& track);
    // void Set(const SharedPlaybackSequence& sequence);

private: /* Members */
    VoidObject* m_Entity;
    SequencerContext* m_Context;

    Type m_Type;

private: /* Methods */
    void RemoveSelected();
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_TAG_WIDGET_H
