// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "SToolbar.h"

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
    m_ResetButton->setText("R");
    // m_ResetButton->setF
    m_ResetButton->setAutoRaise(true);

    m_Layout->addWidget(m_ResetButton);
}

void SToolbar::Connect()
{
    connect(m_ResetButton, &QToolButton::clicked, this, &SToolbar::reset);
}

VOID_NAMESPACE_CLOSE
