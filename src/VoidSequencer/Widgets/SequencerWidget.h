// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_WIDGET_H
#define _SEQUENCER_WIDGET_H

/* Qt */
#include <QLayout>
#include <QShortcut>
#include <QSlider>
#include <QWidget>

/* Internal */
#include "Definition.h"
#include "SMenu.h"
#include "STimelineRuler.h"
#include "STrackHeaderWidget.h"
#include "SToolbar.h"
#include "SVersionSwitcher.h"
#include "VoidSequencer/SContext.h"
#include "VoidSequencer/STimelineGeometry.h"
#include "VoidSequencer/Graphics/STimelineView.h"

VOID_NAMESPACE_OPEN

class SequencerWidget : public QWidget
{
public:
    SequencerWidget(TimelineController* controller, QWidget* parent = nullptr);
    virtual ~SequencerWidget();

    inline QSize sizeHint() const override { return QSize(640, 300); }
    void ResetTabText();

protected:
    void wheelEvent(QWheelEvent* event) override;

protected:
    QHBoxLayout* m_Layout;
    QSlider* m_HZoomSlider;
    SToolbar* m_Toolbar;
    STimelineRuler* m_Ruler;
    STrackHeaderWidget* m_TrackHeader;
    STimelineView* m_View;
    SVersionSwitcher* m_VersionSwitcher;
    SequencerContextMenu* m_Menu;

    QShortcut* m_CutShortcut;
    QShortcut* m_CopyShortcut;
    QShortcut* m_PasteShortcut;
    QShortcut* m_DeleteShortcut;
    QShortcut* m_RippleDeleteShortcut;
    QShortcut* m_ToggleStateShortcut;

    SequencerContext m_Context;

protected: /* Methods */
    void Clear();
    void Build();
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_WIDGET_H
