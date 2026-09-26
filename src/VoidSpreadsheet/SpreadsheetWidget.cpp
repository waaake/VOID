// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "SpreadsheetWidget.h"

VOID_NAMESPACE_OPEN

SpreadsheetWidget::SpreadsheetWidget(QWidget* parent)
    : QWidget(parent)
{
    Build();
    Setup();
}

SpreadsheetWidget::~SpreadsheetWidget()
{
    m_Layout->deleteLater();
    delete m_Layout;
    m_Layout = nullptr;
}

void SpreadsheetWidget::Build()
{
    m_Layout = new QVBoxLayout(this);

    QHBoxLayout* boxlayout = new QHBoxLayout;
    m_MatchMediaBtn = new QPushButton("Match Media");
    m_SetRefMediaBtn = new QPushButton("Set Reference Media");

    boxlayout->addWidget(m_MatchMediaBtn);
    boxlayout->addWidget(m_SetRefMediaBtn);
    boxlayout->addStretch(1);

    m_Sheet = new SpreadsheetTable;

    m_Layout->addLayout(boxlayout);
    m_Layout->addWidget(m_Sheet);
}

void SpreadsheetWidget::Setup()
{

}

VOID_NAMESPACE_CLOSE
