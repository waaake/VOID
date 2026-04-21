// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>
#include <QScrollArea>

/* Internal */
#include "Properties.h"

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

void PropertiesPanel::Build()
{
    m_Layout = new QVBoxLayout(this);

    BaseWidget* scrollWidget = new BaseWidget;
    scrollWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

    m_ScrollLayout = new QVBoxLayout(scrollWidget);
    m_ScrollLayout->setContentsMargins(4, 4, 4, 0);

    QScrollArea* scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(scrollWidget);

    m_Layout->addWidget(scrollArea);
    m_Layout->setContentsMargins(1, 1, 1, 1);
}

VOID_NAMESPACE_CLOSE
