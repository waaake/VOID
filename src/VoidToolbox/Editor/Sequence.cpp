// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <limits>

/* Internal */
#include "Sequence.h"

VOID_NAMESPACE_OPEN

SequenceEditor::SequenceEditor(const SharedPlaybackSequence& sequence, QWidget* parent)
    : BaseEditor(parent)
    , m_Sequence(sequence)
{
    Build();
    Setup();
}

SequenceEditor::~SequenceEditor()
{
}

void SequenceEditor::Build()
{
    m_FramerateCombo = new QComboBox(this);
    m_FrameStartEdit = new IntField(0, std::numeric_limits<int>::max(), this);
    // m_TimeDisplayCombo = new QComboBox(this);
    // m_LayerCombo = new QComboBox();
    // m_PixelAspectEdit = new DoubleField(this);

    AddProperty("Framerate", m_FramerateCombo, 1, -1);
    AddProperty("Start Frame", m_FrameStartEdit, 1, 1);

    AddSeparator();
}

void SequenceEditor::Setup()
{
    SetName(m_Sequence->Name().c_str());
    SetColor(m_Sequence->Color());

    connect(this, &SequenceEditor::nameChangeRequested, this, [this](const QString& name) -> void
    {
        m_Sequence->SetName(name.toStdString());
    });
    connect(this, &SequenceEditor::colorChangeRequested, this, [this](const QColor& color) -> void
    {
        m_Sequence->SetColor(color);
    });
}

VOID_NAMESPACE_CLOSE
