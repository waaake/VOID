// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QPainter>
#include <QSpinBox>

/* Internal */
#include "Effects.h"
#include "VoidUi/Player/PlayerBridge.h"
#include "VoidUi/Engine/IconForge.h"

VOID_NAMESPACE_OPEN

EffectEditor::EffectEditor(Effect* effect, QWidget* parent)
    : QWidget(parent)
    , m_Effect(effect)
{
    Build();
    Setup();
    Connect();
}

EffectEditor::~EffectEditor()
{
    m_Layout->deleteLater();
    delete m_Layout;
    m_Layout = nullptr;
}

void EffectEditor::Close()
{
    hide();
    setParent(nullptr);
    deleteLater();
}

void EffectEditor::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    const QRect r = rect();
    painter.save();

    painter.setPen(QPen(QBrush(Qt::black), 1));
    painter.setBrush(QBrush(palette().color(QPalette::Window)));
    painter.drawRect(r.adjusted(0, 0, -1, -1));

    painter.setBrush(QBrush(palette().color(QPalette::Dark)));
    painter.drawRect(0, 0, r.width() - 1, 30);
    painter.restore();

    QWidget::paintEvent(event);
}

void EffectEditor::Build()
{
    m_Layout = new QVBoxLayout(this);

    QHBoxLayout* title = new QHBoxLayout;
    title->setContentsMargins(2, 0, 6, 8);

    m_NameEdit = new QLineEdit;
    m_NameEdit->setText(m_Effect->Name().c_str());

    m_EnableButton = new QToolButton;
    m_CloseButton = new CloseButton;

    title->addWidget(m_NameEdit);
    title->addWidget(m_EnableButton);
    title->addWidget(m_CloseButton);

    QFormLayout* form = new QFormLayout;

    for (const auto& [name, param] : m_Effect->Params())
    {
        switch (param.type)
        {
            case Param::TypeDesc::Boolean:
            {
                QCheckBox* editor = new QCheckBox;
                editor->setChecked(param.GetBool());
                form->addRow(param.label.c_str(), editor);

                connect(editor, &QCheckBox::toggled, this, [&](bool b) -> void
                {
                    m_Effect->SetValue(name, b);
                    _PlayerBridge.Refresh();
                }, Qt::DirectConnection);
                break;
            }
            case Param::TypeDesc::Float:
            {
                QDoubleSpinBox* editor = new QDoubleSpinBox;
                editor->setValue(param.GetFloat());
                form->addRow(param.label.c_str(), editor);
                editor->setSingleStep(0.1);

                connect(editor, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [&](double d) -> void
                {
                    m_Effect->SetValue(name, static_cast<float>(d));
                    _PlayerBridge.Refresh();
                }, Qt::DirectConnection);
                break;
            }
            case Param::TypeDesc::Int:
            {
                QSpinBox* editor = new QSpinBox;
                editor->setValue(param.GetInt());
                form->addRow(param.label.c_str(), editor);

                connect(editor, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [&](int i) -> void
                {
                    m_Effect->SetValue(name, i);
                    _PlayerBridge.Refresh();
                }, Qt::DirectConnection);
                break;
            }
            case Param::TypeDesc::String:
            default:
            {
                QLineEdit* editor = new QLineEdit;
                editor->setText(param.GetString().c_str());
                form->addRow(param.label.c_str(), editor);

                connect(editor, &QLineEdit::editingFinished, this, [&]() -> void
                {
                    m_Effect->SetValue(name, editor->text().toStdString());
                    _PlayerBridge.Refresh();
                }, Qt::DirectConnection);
                break;
            }
        }
    }

    form->setContentsMargins(8, 0, 8, 8);

    m_Layout->addLayout(title);
    m_Layout->addLayout(form);
}

void EffectEditor::Setup()
{
    int margins[4];
    m_Layout->getContentsMargins(&margins[0], &margins[1], &margins[2], &margins[3]);
    m_Layout->setContentsMargins(1, 4, 1, margins[3]);

    QIcon icon;
    icon.addPixmap(IconForge::GetPixmap(IconType::icon_visible, _DARK_COLOR(QPalette::Text, 100)), QIcon::Normal, QIcon::On);
    icon.addPixmap(IconForge::GetPixmap(IconType::icon_visible_off, _DARK_COLOR(QPalette::Text, 100)), QIcon::Normal, QIcon::Off);

    m_EnableButton->setIcon(icon);
    m_EnableButton->setCheckable(true);
    m_EnableButton->setAutoRaise(true);

    m_EnableButton->setChecked(m_Effect->Enabled());

    m_EnableButton->setFixedSize(16, 16);
    m_CloseButton->setFixedSize(16, 16);

    // Match the Effect Name, for finding the widget corresponding to the effect
    setObjectName(m_Effect->Name().c_str());
}

void EffectEditor::Connect()
{
    connect(m_NameEdit, &QLineEdit::editingFinished, this, [&]() -> void
    {
        const QString text = m_NameEdit->text();
        m_Effect->SetName(text.toStdString());
        setObjectName(text);
    });

    connect(m_CloseButton, &QToolButton::clicked, this, &EffectEditor::Close);
    connect(m_EnableButton, &QToolButton::toggled, this, [&](bool checked) -> void
    {
        m_Effect->SetEnabled(checked);
        _PlayerBridge.Refresh();
    }, Qt::DirectConnection);
}

VOID_NAMESPACE_CLOSE
