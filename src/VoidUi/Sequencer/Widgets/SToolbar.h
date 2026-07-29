// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_TOOLBAR_H
#define _SEQUENCER_TOOLBAR_H

/* Qt */
#include <QButtonGroup>
#include <QFrame>
#include <QLayout>
#include <QToolButton>

/* Internal */
#include "Definition.h"
#include "VoidUi/Sequencer/SContext.h"
#include "VoidUi/QExtensions/PushButton.h"

VOID_NAMESPACE_OPEN

class SToolbar : public QFrame
{
    Q_OBJECT
public:
    explicit SToolbar(QWidget* parent = nullptr);

signals:
    void reset();
    void actionSwitched(const SequencerAction&);

private:
    QVBoxLayout* m_Layout;
    QButtonGroup m_ActionGroup;

    QToolButton* m_ResetButton;
    HighlightToggleButton* m_PointerButton;
    HighlightToggleButton* m_SlipClipButton;
    HighlightToggleButton* m_RazorButton;
    HighlightToggleButton* m_RazorAllButton;
    HighlightToggleButton* m_MergeButton;

private: /* Methods */
    void Build();
    void Setup();
    void Connect();
    void ActionSwitched(int action);
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_TOOLBAR_H
