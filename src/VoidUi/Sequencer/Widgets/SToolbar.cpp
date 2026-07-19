// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "SToolbar.h"
#include "VoidUi/Engine/IconForge.h"

VOID_NAMESPACE_OPEN

SToolbar::SToolbar(QWidget* parent)
    : QFrame(parent)
{
    Build();
    Connect();

    setFixedWidth(40);
}

void SToolbar::Build()
{
    m_Layout = new QVBoxLayout(this);

    m_ResetButton = new QToolButton();
    m_ResetButton->setIcon(IconForge::GetIcon(IconType::icon_sync_alt, _DARK_COLOR(QPalette::Text, 100)));
    m_ResetButton->setAutoRaise(true);

    m_Layout->addWidget(m_ResetButton);
    m_Layout->setAlignment(Qt::AlignTop);
}

void SToolbar::Connect()
{
    connect(m_ResetButton, &QToolButton::clicked, this, &SToolbar::reset);
}

VOID_NAMESPACE_CLOSE
