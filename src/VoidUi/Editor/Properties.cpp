// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>
#include <QScrollArea>

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

    m_ScrollLayout->addWidget(new EffectEditor(effect, this));
}

void PropertiesPanel::Clear()
{
    for (auto w : findChildren<EffectEditor*>())
        w->Close();
}

void PropertiesPanel::Build()
{
    m_Layout = new QVBoxLayout(this);

    QHBoxLayout* optionsLayout = new QHBoxLayout;
    optionsLayout->setContentsMargins(8, 0, 8, 0);
    m_ClearButton = new QPushButton;

    optionsLayout->addWidget(m_ClearButton);
    optionsLayout->addStretch(1);

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
    m_ClearButton->setFixedSize(25, 25);
    m_ClearButton->setIcon(IconForge::GetIcon(IconType::icon_clear_all, _DARK_COLOR(QPalette::Text, 100)));
}

void PropertiesPanel::Connect()
{
    connect(m_ClearButton, &QPushButton::clicked, this, &PropertiesPanel::Clear);
}

VOID_NAMESPACE_CLOSE
