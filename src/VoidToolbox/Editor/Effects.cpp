// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QPainter>
#include <QSpinBox>

/* Internal */
#include "Effects.h"
#include "VoidIconForge/IconForge.h"
#include "VoidQExtensions/Slider.h"

VOID_NAMESPACE_OPEN

EffectEditor::EffectEditor(Effect* effect, QWidget* parent)
    : BaseEditor(parent)
    , m_Effect(effect)
{
    Build();
    Setup();
}

void EffectEditor::Build()
{
    for (const auto& param : m_Effect->Params())
    {
        switch (param->type)
        {
            case Param::TypeDesc::Boolean:
                SetupBoolParam(param);
                break;
            case Param::TypeDesc::Float:
                param->range ? SetupFloatRangedParam(param) : SetupFloatParam(param);
                break;
            case Param::TypeDesc::Int:
                SetupIntParam(param);
                break;
            case Param::TypeDesc::String:
            default:
                SetupStringParam(param);
                break;
        }
    }
}

void EffectEditor::Setup()
{
    SetName(m_Effect->Name().c_str());
    SetColor(m_Effect->Color());

    connect(this, &EffectEditor::nameChangeRequested, this, [this](const QString& name) -> void
    {
        m_Effect->SetName(name.toStdString());
    });

    connect(this, &EffectEditor::stateChangeRequested, this, [this](bool checked) -> void
    {
        m_Effect->SetEnabled(checked);
        // _PlayerBridge.Refresh();
    });
    connect(this, &EffectEditor::colorChangeRequested, m_Effect, &Effect::SetColor);
}

void EffectEditor::SetupIntParam(const Param* param)
{
    QSpinBox* editor = new QSpinBox;
    if (param->range)
    {
        editor->setRange(static_cast<int>(param->range.lower), static_cast<int>(param->range.upper));
        editor->setSingleStep(static_cast<int>(param->range.step));
    }
    editor->setValue(param->GetInt());

    AddControl({ param->name.c_str(), param->label.c_str(), editor });
    connect(editor, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [this, param](int i) -> void
    {
        m_Effect->SetValue(param->name, i);
        // _PlayerBridge.Refresh();
    });
}

void EffectEditor::SetupFloatRangedParam(const Param* param)
{
    QuickDoubleSlider* editor = new QuickDoubleSlider;

    editor->SetRange(param->range.lower, param->range.upper);
    editor->SetSingleStep(param->range.step);

    editor->SetValue(param->GetFloat());

    AddControl({ param->name.c_str(), param->label.c_str(), editor });
    connect(editor, &QuickDoubleSlider::valueChanged, this, [this, param](double d) -> void
    {
        m_Effect->SetValue(param->name, static_cast<float>(d));
        // _PlayerBridge.Refresh();
    });
}

void EffectEditor::SetupFloatParam(const Param* param)
{
    QDoubleSpinBox* editor = new QDoubleSpinBox;
    editor->setSingleStep(0.1);
    editor->setValue(param->GetFloat());

    AddControl({ param->name.c_str(), param->label.c_str(), editor });
    connect(editor, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [this, param](double d) -> void
    {
        m_Effect->SetValue(param->name, static_cast<float>(d));
        // _PlayerBridge.Refresh();
    });
}

void EffectEditor::SetupBoolParam(const Param* param)
{
    QCheckBox* editor = new QCheckBox;
    editor->setChecked(param->GetBool());

    AddControl({ param->name.c_str(), param->label.c_str(), editor });
    connect(editor, &QCheckBox::toggled, this, [this, param](bool b) -> void
    {
        m_Effect->SetValue(param->name, b);
        // _PlayerBridge.Refresh();
    });
}

void EffectEditor::SetupStringParam(const Param* param)
{
    QLineEdit* editor = new QLineEdit;
    editor->setText(param->GetString().c_str());

    AddControl({ param->name.c_str(), param->label.c_str(), editor });
    connect(editor, &QLineEdit::editingFinished, this, [this, param, editor]() -> void
    {
        m_Effect->SetValue(param->name, editor->text().toStdString());
        // _PlayerBridge.Refresh();
    });
}

VOID_NAMESPACE_CLOSE
