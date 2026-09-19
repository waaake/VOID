// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "SMenu.h"
#include "FormatForge.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

SequencerContextMenu::SequencerContextMenu(SequencerContext* context, QWidget* parent)
    : QMenu(parent)
    , m_Context(context)
{
    Build();
    Connect();
}

// SequencerContextMenu::~SequencerContextMenu()
// {
//     m_ColorItemAction->deleteLater();
//     delete
// }

void SequencerContextMenu::Show(const QPoint& position)
{
    m_ExecPosition = position;
    BuildEffectsMenu();
    Validate();
    exec(position);
}

void SequencerContextMenu::Build()
{
    QWidget* p = parentWidget();
    m_NewMenu = new QMenu("New", this);
    m_AddVideoTrackAction = new QAction("Add Video Track", m_NewMenu);
    m_NewMenu->addAction(m_AddVideoTrackAction);

    m_EditMenu = new QMenu("Edit", this);
    m_CutAction = new QAction("Cut", m_EditMenu);
    m_CutAction->setShortcut(QKeySequence::Cut);
    m_CopyAction = new QAction("Copy", m_EditMenu);
    m_CopyAction->setShortcut(QKeySequence::Copy);
    m_PasteAction = new QAction("Paste", m_EditMenu);
    m_PasteAction->setShortcut(QKeySequence::Paste);

    m_RemoveSelectedAction = new QAction("Delete Selected", m_EditMenu);
    m_EditMenu->addAction(m_CutAction);
    m_EditMenu->addAction(m_CopyAction);
    m_EditMenu->addAction(m_PasteAction);
    m_EditMenu->addSeparator();
    m_EditMenu->addAction(m_RemoveSelectedAction);

    m_MarkMenu = new QMenu("Mark", this);
    m_MarkSelectionAction = new QAction("Mark Selection as Timeline in/out", m_MarkMenu);
    m_MarkSelectionAction->setShortcut(QKeySequence("Alt+U"));
    m_MarkCurrentAction = new QAction("Mark Current Clip as Timeline in/out", m_MarkMenu);
    m_MarkCurrentAction->setShortcut(Qt::Key_U);

    p->addAction(m_MarkSelectionAction);
    p->addAction(m_MarkCurrentAction);
    m_MarkMenu->addAction(m_MarkSelectionAction);
    m_MarkMenu->addAction(m_MarkCurrentAction);

    m_ColorMenu = new QMenu("Color", this);

    m_ColorItemAction = new QAction("Set Trackitem Color...", m_ColorMenu);
    m_ResetItemColorAction = new QAction("Reset Trackitem Color", m_ColorMenu);
    m_ColorMenu->addAction(m_ColorItemAction);
    m_ColorMenu->addAction(m_ResetItemColorAction);

    m_VersionMenu = new QMenu("Version", this);
    m_InspectVersionsAction = new QAction("Inspect Versions", m_VersionMenu);
    m_InspectVersionsAction->setShortcut(QKeySequence(Qt::Key_V));
    m_ScanDirectoryAction = new QAction("Scan for Versions", m_VersionMenu);
    m_VersionUpAction = new QAction("Version Up", m_VersionMenu);
    m_VersionUpAction->setShortcut(QKeySequence("Alt+Up"));
    m_VersionDownAction = new QAction("Version Down", m_VersionMenu);
    m_VersionDownAction->setShortcut(QKeySequence("Alt+Down"));
    m_MaxVersionAction = new QAction("Max Version", m_VersionMenu);
    m_MaxVersionAction->setShortcut(QKeySequence("Alt+Shift+Up"));
    m_MinVersionAction = new QAction("Min Version", m_VersionMenu);
    m_MinVersionAction->setShortcut(QKeySequence("Alt+Shift+Down"));
    p->addAction(m_InspectVersionsAction);
    p->addAction(m_VersionUpAction);
    p->addAction(m_VersionDownAction);
    m_VersionMenu->addAction(m_InspectVersionsAction);
    m_VersionMenu->addAction(m_ScanDirectoryAction);
    m_VersionMenu->addAction(m_VersionUpAction);
    m_VersionMenu->addAction(m_VersionDownAction);
    m_VersionMenu->addAction(m_MaxVersionAction);
    m_VersionMenu->addAction(m_MinVersionAction);

    m_EditModeMenu = new QMenu("Edit Mode", this);
    m_EditModeGroup = new QActionGroup(m_EditModeMenu);

    m_NoOverwriteAction = new QAction("Don't Overwrite Existing Items", m_EditModeMenu);
    m_NoOverwriteAction->setCheckable(true);
    m_NoOverwriteAction->setData(static_cast<int>(SequencerController::EditMode::NO_OVERWRITE));
    m_OverwriteAction = new QAction("Overwrite Existing Items", m_EditModeMenu);
    m_OverwriteAction->setCheckable(true);
    m_OverwriteAction->setData(static_cast<int>(SequencerController::EditMode::OVERWRITE));
    m_RippleAction = new QAction("Ripple Edit", m_EditModeMenu);
    m_RippleAction->setCheckable(true);
    m_RippleAction->setData(static_cast<int>(SequencerController::EditMode::RIPPLE));

    m_EditModeGroup->setExclusive(true);
    m_EditModeGroup->addAction(m_NoOverwriteAction);
    m_EditModeGroup->addAction(m_OverwriteAction);
    m_EditModeGroup->addAction(m_RippleAction);

    m_EditModeMenu->addAction(m_NoOverwriteAction);
    m_EditModeMenu->addAction(m_OverwriteAction);
    m_EditModeMenu->addAction(m_RippleAction);

    m_EffectsMenu = new QMenu("Timeline Effects", this);

    addMenu(m_NewMenu);
    addMenu(m_EditMenu);
    addMenu(m_MarkMenu);
    addSeparator();
    addMenu(m_ColorMenu);
    addMenu(m_VersionMenu);
    addSeparator();
    addMenu(m_EditModeMenu);
    addSeparator();
    addMenu(m_EffectsMenu);
}

void SequencerContextMenu::Connect()
{
    connect(m_AddVideoTrackAction, &QAction::triggered, this, &SequencerContextMenu::createTrackRequested);
    connect(m_CutAction, &QAction::triggered, this, &SequencerContextMenu::cutSelectionRequested);
    connect(m_CopyAction, &QAction::triggered, this, &SequencerContextMenu::copySelectionRequested);
    connect(m_PasteAction, &QAction::triggered, this, [this]() -> void { emit pasteRequested(m_ExecPosition); });
    connect(m_RemoveSelectedAction, &QAction::triggered, this, &SequencerContextMenu::deleteSelectionRequested);
    connect(m_ColorItemAction, &QAction::triggered, this, [this]() -> void { emit colorChangeRequested(false); });
    connect(m_ResetItemColorAction, &QAction::triggered, this, [this]() -> void { emit colorChangeRequested(true); });
    connect(m_EditModeGroup, &QActionGroup::triggered, this, [this](QAction* action) -> void
    {
        emit editModeChangeRequested(static_cast<SequencerController::EditMode>(action->data().toInt()));
    });
    connect(m_VersionUpAction, &QAction::triggered, this, [this]() -> void { emit versionChangeRequested(true); });
    connect(m_VersionDownAction, &QAction::triggered, this, [this]() -> void { emit versionChangeRequested(false); });
    connect(m_MaxVersionAction, &QAction::triggered, this, [this]() -> void { emit versionExtremesChangeRequested(true); });
    connect(m_MinVersionAction, &QAction::triggered, this, [this]() -> void { emit versionExtremesChangeRequested(false); });
    connect(m_InspectVersionsAction, &QAction::triggered, this, &SequencerContextMenu::versionInspectionRequested);
    connect(m_ScanDirectoryAction, &QAction::triggered, this, &SequencerContextMenu::versionScanRequested);
    connect(m_MarkSelectionAction, &QAction::triggered, this, [this]() -> void { emit inOutSetRequested(true); });
    connect(m_MarkCurrentAction, &QAction::triggered, this, [this]() -> void { emit inOutSetRequested(false); });
}

void SequencerContextMenu::Validate()
{
    const SSelectionModel* sel = m_Context->SelectionModel();
    const SequencerController* controller = m_Context->Controller();
    const bool anySelection = sel->HasAnySelection();
    const bool itemSelection = sel->HasTrackItemSelection();
    const bool trackSelection = sel->HasTrackSelection();

    setEnabled(m_Context->HasActiveSequence());

    m_CutAction->setEnabled(anySelection);
    m_CopyAction->setEnabled(anySelection);
    m_PasteAction->setEnabled(controller->ValidClipboard());

    m_MarkSelectionAction->setEnabled(itemSelection);

    m_RemoveSelectedAction->setEnabled(anySelection);
    m_ColorItemAction->setEnabled(itemSelection);
    m_ResetItemColorAction->setEnabled(itemSelection);

    m_InspectVersionsAction->setEnabled(itemSelection);
    m_ScanDirectoryAction->setEnabled(itemSelection);
    m_VersionUpAction->setEnabled(itemSelection);
    m_VersionDownAction->setEnabled(itemSelection);
    m_MinVersionAction->setEnabled(itemSelection);
    m_MaxVersionAction->setEnabled(itemSelection);

    m_NoOverwriteAction->setChecked(controller->GetEditMode() == SequencerController::EditMode::NO_OVERWRITE);
    m_OverwriteAction->setChecked(controller->GetEditMode() == SequencerController::EditMode::OVERWRITE);
    m_RippleAction->setChecked(controller->GetEditMode() == SequencerController::EditMode::RIPPLE);

    m_EffectsMenu->setEnabled(itemSelection || trackSelection);
}

void SequencerContextMenu::BuildEffectsMenu()
{
    if (m_EffectsMenu->actions().empty())
    {
        for (const auto& [name, _] : Forge::Instance().Operators())
        {
            VOID_LOG_INFO("Effect: {0}", name);
            QAction* effaction = new QAction(name.c_str(), m_EffectsMenu);
            connect(effaction, &QAction::triggered, this, [=]() -> void { emit addEffectRequested(name); });
            m_EffectsMenu->addAction(effaction);
        }
    }
}

VOID_NAMESPACE_CLOSE
