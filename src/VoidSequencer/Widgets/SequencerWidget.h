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
#include "SToolbar.h"
#include "SVersionSwitcher.h"
#include "VoidSequencer/SContext.h"
#include "VoidSequencer/STimelineGeometry.h"
#include "VoidSequencer/Graphics/STimelineView.h"
#include "VoidSequencer/Graphics/SHeaderView.h"

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
    STimelineView* m_View;
    SHeaderView* m_Header;
    SVersionSwitcher* m_VersionSwitcher;
    SequencerContextMenu* m_Menu;

    SequencerContext m_Context;

protected: /* Methods */
    void Clear();
    void Build();
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_WIDGET_H
