// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_H
#define _SEQUENCER_H

/* Qt */
#include <QLayout>
#include <QShortcut>
#include <QSlider>
#include <QWidget>

/* Internal */
#include "Definition.h"
#include "Descriptors.h"
#include "VoidObjects/Sequence/Sequence.h"
#include "VoidSequencer/Widgets/SequencerWidget.h"

VOID_NAMESPACE_OPEN

class VOID_API SequencerTimeline : public SequencerWidget
{
    Q_OBJECT
public:
    explicit SequencerTimeline(TimelineController* controller, QWidget* parent = nullptr);
    ~SequencerTimeline();

    void SetSequence(const SharedPlaybackSequence& sequence);
    void ClearSequence();
    SharedPlaybackSequence ActiveSequence() const { return m_Context.Sequence(); }

    void AddTrack(const SharedPlaybackTrack& track);
    void RemoveTrack(const SharedPlaybackTrack& track);
    void RazorAt(v_frame_t frame);
    void RazorAt(const SharedPlaybackSequence& sequence, v_frame_t frame);
    void RazorAt(const SharedPlaybackTrack& track, v_frame_t frame);
    void MergeCut(const SharedPlaybackTrack& track, v_frame_t frame);
    void TrimItemHead(const SharedTrackItem& item, int handle);
    void TrimItemTail(const SharedTrackItem& item, int handle);

    void SetHorizontalScale(float factor);
    void Refresh();
    void FitAll();
    void FitSelected();
    void ResetFit();

signals:
    void editEffectRequested(Effect*);

private: /* Methods */
    void Connect();
    void Connect(PlaybackSequence* sequence);
    void Disconnect(PlaybackSequence* sequence);
    void CreateEffect(const std::string& type);
    void DeleteSelected();
    void RippleDeleteSelected();
    void ToggleItemState();
    void UpdateAll();
    void Cut();
    void Copy();
    void Paste(const QPoint& position);
    void SwitchVersion(bool up);
    void SwitchVersionExtremes(bool max);
    void InspectVersions();
    void ScanVersions();
    void ResetInOut(bool selection = false);
    void Razor(bool sequence = false);
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_H
