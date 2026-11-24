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
    m_FramerateCtrl = new FramerateBox(this);
    m_FrameStartEdit = new IntField(0, std::numeric_limits<int>::max(), this);
    // m_TimeDisplayCombo = new QComboBox(this);
    // m_LayerCombo = new QComboBox();
    // m_PixelAspectEdit = new DoubleField(this);

    AddProperty("Framerate", m_FramerateCtrl, 1, -1);
    AddProperty("Start Frame", m_FrameStartEdit, 1, 1);

    AddSeparator();
}

void SequenceEditor::Setup()
{
    SetName(m_Sequence->Name().c_str());
    SetColor(m_Sequence->Color());

    m_FramerateCtrl->SetFramerate(m_Sequence->Framerate());
    m_FrameStartEdit->SetValue(m_Sequence->StartFrame());

    connect(this, &SequenceEditor::nameChangeRequested, this, [this](const QString& name) -> void
    {
        m_Sequence->SetName(name.toStdString());
    });
    connect(this, &SequenceEditor::colorChangeRequested, this, [this](const QColor& color) -> void
    {
        m_Sequence->SetColor(color);
    });
    connect(m_FramerateCtrl, &FramerateBox::framerateChanged, this, [this](double fr) -> void
    {
        m_Sequence->SetFramerate(fr);
    });
    connect(m_FrameStartEdit, &IntField::valueChanged, this, [this](int f) -> void
    {
        m_Sequence->SetStartFrame(f);
    });
}

VOID_NAMESPACE_CLOSE
