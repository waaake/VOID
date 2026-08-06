// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_CONTEXT_MENU_H
#define _SEQUENCER_CONTEXT_MENU_H

/* Qt */
#include <QMenu>
#include <QAction>
#include <QActionGroup>

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
    void removeTracksRequested();
    void removeTrackItemsRequested();
    void colorChangeRequested(bool reset = false);
    void editModeChangeRequested(const SequencerController::EditMode&);

private:
    SequencerContext* m_Context;
    QAction* m_AddVideoTrackAction;
    QAction* m_RemoveTrackAction;
    QAction* m_RemoveTrackItemsAction;

    QMenu* m_ColorMenu;
    QAction* m_ColorItemAction;
    QAction* m_ResetItemColorAction;

    QMenu* m_EditModeMenu;
    QAction* m_NoOverwriteAction;
    QAction* m_OverwriteAction;
    QAction* m_RippleAction;
    QActionGroup* m_EditModeGroup;

private: /* Methods */
    void Build();
    void Connect();
    void Validate();
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_CONTEXT_MENU_H
