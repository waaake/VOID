// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "MetadataViewer.h"

VOID_NAMESPACE_OPEN

MetadataViewer::MetadataViewer(QWidget* parent)
    : QWidget(parent)
{
    Build();
}

MetadataViewer::~MetadataViewer()
{
    m_Layout->deleteLater();
    delete m_Layout;
    m_Layout = nullptr;
}

void MetadataViewer::Build()
{
    m_Layout = new QVBoxLayout(this);

    m_ButtonLayout = new QHBoxLayout();
    m_RefreshButton = new QPushButton("Refresh");

    m_ButtonLayout->addStretch(1);
    m_ButtonLayout->addWidget(m_RefreshButton);

    m_Tree = new MetadataTree(this);

    m_Layout->addLayout(m_ButtonLayout);
    m_Layout->addWidget(m_Tree);
}

VOID_NAMESPACE_CLOSE
