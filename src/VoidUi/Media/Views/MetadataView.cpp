// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "MetadataView.h"

VOID_NAMESPACE_OPEN

MetadataTree::MetadataTree(QWidget* parent)
    : QTreeView(parent)
{
    m_Metadata = new MetadataModel(this);
    Setup();
}

MetadataTree::~MetadataTree()
{
    m_Metadata->deleteLater();
    delete m_Metadata;
    m_Metadata = nullptr;
}

void MetadataTree::Setup()
{
    setAlternatingRowColors(true);
    setModel(m_Metadata);
}

VOID_NAMESPACE_CLOSE
