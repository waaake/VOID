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
#include "VoidSequencer/SContext.h"

VOID_NAMESPACE_OPEN

class SequencerContextMenu : public QMenu
{
    Q_OBJECT
public:
    SequencerContextMenu(SequencerContext* context, QWidget* parent = nullptr);
    void Show(const QPoint& position);

signals:
    /// New
    void createTrackRequested(const Sequence::Type&);

    /// Edit
    void cutSelectionRequested();
    void copySelectionRequested();
    void pasteRequested(const QPoint&);
    void deleteSelectionRequested();
    void renameRequested();

    /// Color
    void colorChangeRequested(bool reset = false);

    /// View
    void fitAllRequested();
    void fitSelectedRequested();
    void resetFitRequested();

    /// Mark
    void inOutSetRequested(bool selection = false);

    /// Version
    void versionChangeRequested(bool up);
    void versionExtremesChangeRequested(bool max);
    void versionInspectionRequested();
    void versionScanRequested();

    /// Tags
    void addTagRequested();
    void editTagsRequested();

    /// Edit Mode
    void editModeChangeRequested(const SequencerController::EditMode&);

    /// Editorial
    void disableRequested();
    void razorRequested(bool sequence = false);
    void rippleDeleteRequested();

    /// Effects
    void addEffectRequested(const std::string&);

private:
    QPoint m_ExecPosition;
    SequencerContext* m_Context;

    QMenu* m_NewMenu;
    QAction* m_AddVideoTrackAction;
    QAction* m_AddAudioTrackAction;

    QMenu* m_EditMenu;
    QAction* m_CutAction;
    QAction* m_CopyAction;
    QAction* m_PasteAction;
    QAction* m_RemoveSelectedAction;
    QAction* m_RenameTrackAction;

    QMenu* m_ViewMenu;
    QAction* m_FitAllAction;
    QAction* m_FitSelectedAction;
    QAction* m_ResetFitAction;

    QMenu* m_MarkMenu;
    QAction* m_MarkSelectionAction;
    QAction* m_MarkCurrentAction;

    QMenu* m_ColorMenu;
    QAction* m_ColorItemAction;
    QAction* m_ResetItemColorAction;

    QMenu* m_VersionMenu;
    QAction* m_ScanDirectoryAction;
    QAction* m_InspectVersionsAction;
    QAction* m_VersionUpAction;
    QAction* m_VersionDownAction;
    QAction* m_MaxVersionAction;
    QAction* m_MinVersionAction;

    QMenu* m_TagsMenu;
    QAction* m_AddTagAction;
    QAction* m_EditTagsAction;

    QMenu* m_EditModeMenu;
    QAction* m_NoOverwriteAction;
    QAction* m_OverwriteAction;
    QAction* m_RippleAction;
    QActionGroup* m_EditModeGroup;

    QMenu* m_EditorialMenu;
    QAction* m_DisableAction;
    QAction* m_RippleDeleteAction;
    QAction* m_RazorAction;
    QAction* m_RazorAllAction;

    QMenu* m_EffectsMenu;

private: /* Methods */
    void Build();
    void BuildEffectsMenu();
    void Connect();
    void Validate();
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_CONTEXT_MENU_H
