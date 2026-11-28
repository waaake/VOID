// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QSignalBlocker>

/* Internal */
#include "Color.h"
#include "VoidColor/ColorEngine.h"

VOID_NAMESPACE_OPEN

ColorConfigEditor::ColorConfigEditor(ColorEngine* engine, QWidget* parent)
    : BaseEditor(parent)
    , m_Engine(engine)
{
    Build();
    Setup();
}

ColorConfigEditor::~ColorConfigEditor()
{
    m_WorkingSpaceCombo->deleteLater();
    delete m_WorkingSpaceCombo;
    m_WorkingSpaceCombo = nullptr;

    m_DisplayCombo->deleteLater();
    delete m_DisplayCombo;
    m_DisplayCombo = nullptr;

    m_ViewCombo->deleteLater();
    delete m_ViewCombo;
    m_ViewCombo = nullptr;

    m_InverseCheckBox->deleteLater();
    delete m_InverseCheckBox;
    m_InverseCheckBox = nullptr;
}

void ColorConfigEditor::Refresh()
{
    m_WorkingSpaceCombo->addItems(m_Engine->Roles());
    m_DisplayCombo->addItems(m_Engine->Displays());
    m_ViewCombo->addItems(m_Engine->Views());

    m_WorkingSpaceCombo->setCurrentText(m_Engine->WorkingSpace());
    m_DisplayCombo->setCurrentText(m_Engine->ActiveDisplay());
    m_ViewCombo->setCurrentText(m_Engine->ActiveView());
}

void ColorConfigEditor::Build()
{
    m_WorkingSpaceCombo = new QComboBox;
    m_DisplayCombo = new QComboBox;
    m_ViewCombo = new QComboBox;
    m_InverseCheckBox = new QCheckBox("Inverse");

    AddControl({ "working_space", "Working Space", m_WorkingSpaceCombo });
    AddControl({ "display", "Display", m_DisplayCombo });
    AddControl({ "view", "View", m_ViewCombo });
    AddControl({ "inverse", m_InverseCheckBox });
}

void ColorConfigEditor::Setup()
{
    SetName("OCIODisplaySettings", true);
    Refresh();

    // To ColorEngine
    connect(m_DisplayCombo, &QComboBox::currentTextChanged, this, [this](const QString& display) -> void
    {
        m_Engine->SetDisplay(display);
        const QSignalBlocker blocker(m_ViewCombo);
        m_ViewCombo->clear();
        m_ViewCombo->addItems(m_Engine->Views());
        m_ViewCombo->setCurrentText(m_Engine->ActiveView());
    });
    connect(m_ViewCombo, &QComboBox::currentTextChanged, m_Engine, &ColorEngine::SetView);

    // From ColorEngine
    connect(m_Engine, &ColorEngine::viewChanged, this, [this]() -> void
    {
        const QSignalBlocker blocker(m_ViewCombo);
        m_ViewCombo->setCurrentText(m_Engine->ActiveView());
    });
}

VOID_NAMESPACE_CLOSE
