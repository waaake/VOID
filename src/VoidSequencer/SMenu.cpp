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
    BuildEffectsMenu();
    Validate();
    exec(position);
}

void SequencerContextMenu::Build()
{
    m_AddVideoTrackAction = new QAction("Add Video Track", this);
    m_RemoveSelectedAction = new QAction("Delete Selected", this);

    m_ColorMenu = new QMenu("Color", this);

    m_ColorItemAction = new QAction("Set Trackitem Color...", m_ColorMenu);
    m_ResetItemColorAction = new QAction("Reset Trackitem Color", m_ColorMenu);
    m_ColorMenu->addAction(m_ColorItemAction);
    m_ColorMenu->addAction(m_ResetItemColorAction);

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

    addAction(m_AddVideoTrackAction);
    addAction(m_RemoveSelectedAction);

    addSeparator();

    addMenu(m_ColorMenu);

    addSeparator();
    
    addMenu(m_EditModeMenu);

    addSeparator();

    addMenu(m_EffectsMenu);
}

void SequencerContextMenu::Connect()
{
    connect(m_AddVideoTrackAction, &QAction::triggered, this, &SequencerContextMenu::createTrackRequested);
    connect(m_RemoveSelectedAction, &QAction::triggered, this, &SequencerContextMenu::deleteSelectionRequested);
    connect(m_ColorItemAction, &QAction::triggered, this, [this]() -> void { emit colorChangeRequested(false); });
    connect(m_ResetItemColorAction, &QAction::triggered, this, [this]() -> void { emit colorChangeRequested(true); });
    connect(m_EditModeGroup, &QActionGroup::triggered, this, [this](QAction* action) -> void
    {
        emit editModeChangeRequested(static_cast<SequencerController::EditMode>(action->data().toInt()));
    });
}

void SequencerContextMenu::Validate()
{
    const SSelectionModel* sel = m_Context->SelectionModel();
    const SequencerController* controller = m_Context->Controller();

    m_RemoveSelectedAction->setEnabled(sel->HasAnySelection());
    m_ColorItemAction->setEnabled(sel->HasTrackItemSelection());
    m_ResetItemColorAction->setEnabled(sel->HasTrackItemSelection());

    m_NoOverwriteAction->setChecked(controller->GetEditMode() == SequencerController::EditMode::NO_OVERWRITE);
    m_OverwriteAction->setChecked(controller->GetEditMode() == SequencerController::EditMode::OVERWRITE);
    m_RippleAction->setChecked(controller->GetEditMode() == SequencerController::EditMode::RIPPLE);

    m_EffectsMenu->setEnabled(sel->HasTrackItemSelection() || sel->HasTrackSelection());
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
