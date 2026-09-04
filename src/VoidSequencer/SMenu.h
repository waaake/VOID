// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_CONTEXT_MENU_H
#define _SEQUENCER_CONTEXT_MENU_H

/* Qt */
#include <QAction>
#include <QActionGroup>
#include <QMenu>

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
    void cutSelectionRequested();
    void copySelectionRequested();
    void pasteRequested(const QPoint&);
    void createTrackRequested();
    void deleteSelectionRequested();
    void colorChangeRequested(bool reset = false);
    void editModeChangeRequested(const SequencerController::EditMode&);
    void addEffectRequested(const std::string&);

private:
    QPoint m_ExecPosition;
    SequencerContext* m_Context;
    QAction* m_AddVideoTrackAction;

    QMenu* m_NewMenu;
    QMenu* m_EditMenu;
    QAction* m_CutAction;
    QAction* m_CopyAction;
    QAction* m_PasteAction;
    QAction* m_RemoveSelectedAction;

    QMenu* m_ColorMenu;
    QAction* m_ColorItemAction;
    QAction* m_ResetItemColorAction;

    QMenu* m_EditModeMenu;
    QAction* m_NoOverwriteAction;
    QAction* m_OverwriteAction;
    QAction* m_RippleAction;
    QActionGroup* m_EditModeGroup;

    QMenu* m_EffectsMenu;

private: /* Methods */
    void Build();
    void BuildEffectsMenu();
    void Connect();
    void Validate();
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_CONTEXT_MENU_H
