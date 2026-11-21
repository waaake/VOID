// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <limits>

/* Internal */
#include "Media.h"
#include "VoidCore/Processors/ColorProcessor.h"

VOID_NAMESPACE_OPEN

MediaEditor::MediaEditor(const SharedMediaClip& media, QWidget* parent)
    : BaseEditor(parent)
    , m_Clip(media)
{
    Build();
    Setup();
}

MediaEditor::~MediaEditor()
{
    m_LayerCombo->deleteLater();
    delete m_LayerCombo;
    m_LayerCombo = nullptr;
}

void MediaEditor::Build()
{
    m_PathEdit = new QLineEdit(this);

    m_RangeStartEdit = new IntField(0, std::numeric_limits<int>::max(), this);
    m_RangeEndEdit = new IntField(0, std::numeric_limits<int>::max(), this);

    m_ColorspaceCombo = new QComboBox(this);
    m_LayerCombo = new QComboBox();
    m_PixelAspectEdit = new DoubleField(this);

    AddProperty("File", m_PathEdit, 1, -1);
    AddProperty("Frame Range", m_RangeStartEdit, 1, 1);
    AddWidget(m_RangeEndEdit, 1, 1, false);
    AddProperty("Pixel Aspect", m_PixelAspectEdit, 1, 1);
    AddProperty("Input Color Transform", m_ColorspaceCombo, 1, -1);

    AddSeparator();

    /// Only the media that have multiple layers, gets this property added
    if (m_Clip->MultiLayered())
        AddProperty("Layer", m_LayerCombo, 1, -1);
}

void MediaEditor::Setup()
{
    SetName(m_Clip->Name().c_str());
    SetColor(m_Clip->Color());

    m_PathEdit->setText(m_Clip->TemplatedPath().c_str());

    // Item Range
    m_RangeStartEdit->SetValue(m_Clip->FirstFrame());
    m_RangeEndEdit->SetValue(m_Clip->LastFrame());

    /// TODO: Right now we don't support editable path and also frame ranges, later we may add the concept of playable range
    /// which can be a subset of the original range
    m_PathEdit->setReadOnly(true);
    m_RangeStartEdit->setReadOnly(true);
    m_RangeEndEdit->setReadOnly(true);
    
    for (const auto& colorspace : MediaColorProcessor::Instance().Colorspaces())
        m_ColorspaceCombo->addItem(colorspace.c_str());

    for (const auto& layer : m_Clip->Layers())
        m_LayerCombo->addItem(layer.c_str());

    // Current layer
    m_LayerCombo->setCurrentText(m_Clip->CurrentLayer().c_str());

    // Current input transform
    m_ColorspaceCombo->setCurrentText(m_Clip->InputColorTransform().c_str());
    m_PixelAspectEdit->SetValue(m_Clip->PixelAspect());

    // Signals
    connect(this, &MediaEditor::colorChangeRequested, this, [this](const QColor& color) -> void
    {
        m_Clip->SetColor(color);
    });
    connect(m_LayerCombo, &QComboBox::currentTextChanged, this, [this](const QString& text) -> void
    {
        m_Clip->SetCurrentLayer(text.toStdString());
    });
    connect(m_ColorspaceCombo, &QComboBox::currentTextChanged, this, [this](const QString& text) -> void
    {
        m_Clip->SetInputColorspace(text.toStdString());
    });

    connect(m_PixelAspectEdit, &DoubleField::valueChanged, this, [this](double v) -> void { m_Clip->SetPixelAspect(v); });
}

VOID_NAMESPACE_CLOSE
