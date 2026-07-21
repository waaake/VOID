// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_CONTEXT_MENU_H
#define _SEQUENCER_CONTEXT_MENU_H

/* Qt */
#include <QMenu>
#include <QAction>

/* Internal */
#include "Definition.h"
#include "SContext.h"

VOID_NAMESPACE_OPEN

class SequencerContextMenu : public QMenu
{
    Q_OBJECT
public:
    SequencerContextMenu(SequencerContext* context, QWidget* parent = nullptr);
    // ~SequencerContextMenu()
    void Show(const QPoint& position);

signals:
    void createTrackRequested();
    void colorChangeRequested(bool reset = false);

private:
    SequencerContext* m_Context;
    QAction* m_AddVideoTrackAction;

    QMenu* m_ColorMenu;
    QAction* m_ColorItemAction;
    QAction* m_ResetItemColorAction;

private: /* Methods */
    void Build();
    void Connect();
    void Validate();
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_CONTEXT_MENU_H
