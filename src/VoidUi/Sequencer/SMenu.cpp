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
    m_ColorMenu = new QMenu("Color", this);

    m_ColorItemAction = new QAction("Set Trackitem Color...", m_ColorMenu);
    m_ResetItemColorAction = new QAction("Reset Trackitem Color", m_ColorMenu);
    m_ColorMenu->addAction(m_ColorItemAction);
    m_ColorMenu->addAction(m_ResetItemColorAction);

    addMenu(m_ColorMenu);
}

void SequencerContextMenu::Connect()
{
    connect(m_ColorItemAction, &QAction::triggered, this, [this]() -> void { emit colorChangeRequested(false); });
    connect(m_ResetItemColorAction, &QAction::triggered, this, [this]() -> void { emit colorChangeRequested(true); });
}

void SequencerContextMenu::Validate()
{
    m_ColorItemAction->setEnabled(m_Context->SelectionModel()->HasSelection());
    m_ResetItemColorAction->setEnabled(m_Context->SelectionModel()->HasSelection());
}

VOID_NAMESPACE_CLOSE
