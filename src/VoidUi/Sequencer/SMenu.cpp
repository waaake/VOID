// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "SMenu.h"

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
    Validate();
    exec(position);
}

void SequencerContextMenu::Build()
{
    m_AddVideoTrackAction = new QAction("Add Video Track", this);
    m_RemoveTrackAction = new QAction("Remove Selected Track(s)", this);

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

    addAction(m_AddVideoTrackAction);
    addAction(m_RemoveTrackAction);

    addSeparator();

    addMenu(m_ColorMenu);

    addSeparator();
    
    addMenu(m_EditModeMenu);
}

void SequencerContextMenu::Connect()
{
    connect(m_AddVideoTrackAction, &QAction::triggered, this, &SequencerContextMenu::createTrackRequested);
    connect(m_RemoveTrackAction, &QAction::triggered, this, &SequencerContextMenu::removeTracksRequested);
    connect(m_ColorItemAction, &QAction::triggered, this, [this]() -> void { emit colorChangeRequested(false); });
    connect(m_ResetItemColorAction, &QAction::triggered, this, [this]() -> void { emit colorChangeRequested(true); });
    connect(m_EditModeGroup, &QActionGroup::triggered, this, [this](QAction* action) -> void
    {
        emit editModeChangeRequested(static_cast<SequencerController::EditMode>(action->data().toInt()));
    });
}

void SequencerContextMenu::Validate()
{
    m_RemoveTrackAction->setEnabled(m_Context->SelectionModel()->HasTrackSelection());
    m_ColorItemAction->setEnabled(m_Context->SelectionModel()->HasSelection());
    m_ResetItemColorAction->setEnabled(m_Context->SelectionModel()->HasSelection());

    m_NoOverwriteAction->setChecked(m_Context->Controller()->GetEditMode() == SequencerController::EditMode::NO_OVERWRITE);
    m_OverwriteAction->setChecked(m_Context->Controller()->GetEditMode() == SequencerController::EditMode::OVERWRITE);
    m_RippleAction->setChecked(m_Context->Controller()->GetEditMode() == SequencerController::EditMode::RIPPLE);
}

VOID_NAMESPACE_CLOSE
