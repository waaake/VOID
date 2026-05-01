// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>
#include <QScrollArea>
#include <QValidator>

/* Internal */
#include "Properties.h"
#include "VoidUi/Engine/IconForge.h"

VOID_NAMESPACE_OPEN

/* Scroll Area {{{ */

class BaseWidget : public QWidget
{
protected:
    void paintEvent(QPaintEvent* event) override
    {
        QPainter painter(this);
        painter.fillRect(rect(), palette().color(QPalette::Base));
    }
};

/* }}} */

PropertiesPanel::PropertiesPanel(QWidget* parent)
    : QWidget(parent)
{
    Build();
    Setup();
    Connect();
}

PropertiesPanel::~PropertiesPanel()
{
    m_ScrollLayout->deleteLater();
    delete m_ScrollLayout;
    m_ScrollLayout = nullptr;

    m_Layout->deleteLater();
    delete m_Layout;
    m_Layout = nullptr;
}

void PropertiesPanel::Show(QWidget* panel)
{
    m_ScrollLayout->addWidget(panel);
}

void PropertiesPanel::EditEffect(Effect* effect)
{
    // Remove any existing Editors for the same Effect before adding this again
    for (auto w : findChildren<EffectEditor*>(effect->Name().c_str()))
        w->Close();

    // Ensure we have only panels till the limit
    // Going one lesser here as we are also adding another widget
    ClearAdditionalPanels(m_PanelCounter->CurrentValue() - 1);
    m_ScrollLayout->insertWidget(0, new EffectEditor(effect, this));
}

void PropertiesPanel::Clear()
{
    for (auto w : findChildren<EffectEditor*>())
        w->Close();
}

void PropertiesPanel::Remove(const std::string& name)
{
    if (auto w = findChild<EffectEditor*>(name.c_str()))
        w->Close();
}

void PropertiesPanel::ClearAdditionalPanels(int limit)
{
    QList<EffectEditor*> panels = findChildren<EffectEditor*>();
    if (panels.size() >= limit)
    {
        for (int i = 0; i < panels.size() - limit; ++i)
            panels.at(i)->Close();
    }
}

void PropertiesPanel::Build()
{
    m_Layout = new QVBoxLayout(this);

    QHBoxLayout* optionsLayout = new QHBoxLayout;
    optionsLayout->setContentsMargins(8, 0, 8, 0);
    m_ClearButton = new QPushButton;
    m_PanelCounter = new IntBoundLineEdit(1, 20, 10, this);

    optionsLayout->addWidget(m_PanelCounter);
    optionsLayout->addStretch(1);
    optionsLayout->addWidget(m_ClearButton);

    BaseWidget* scrollWidget = new BaseWidget;
    scrollWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

    m_ScrollLayout = new QVBoxLayout(scrollWidget);
    m_ScrollLayout->setContentsMargins(4, 4, 4, 0);

    QScrollArea* scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(scrollWidget);

    m_Layout->addLayout(optionsLayout);
    m_Layout->addWidget(scrollArea);
    m_Layout->setContentsMargins(1, 8, 1, 1);
}

void PropertiesPanel::Setup()
{
    m_PanelCounter->setFixedWidth(25);

    m_ClearButton->setFixedSize(25, 25);
    m_ClearButton->setIcon(IconForge::GetIcon(IconType::icon_clear_all, _DARK_COLOR(QPalette::Text, 100)));
}

void PropertiesPanel::Connect()
{
    connect(m_ClearButton, &QPushButton::clicked, this, &PropertiesPanel::Clear);
    connect(m_PanelCounter, &IntBoundLineEdit::currentValueChanged, this, &PropertiesPanel::ClearAdditionalPanels);
}

VOID_NAMESPACE_CLOSE
