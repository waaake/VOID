// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "MetadataView.h"

VOID_NAMESPACE_OPEN

MetadataTree::MetadataTree(QWidget* parent)
    : QTreeView(parent)
{
    m_Metadata = new MetadataModel(this);
    m_MetadataProxy = new MetadataSortProxyModel(this);
    Setup();
}

MetadataTree::~MetadataTree()
{
    m_Metadata->deleteLater();
    delete m_Metadata;
    m_Metadata = nullptr;

    m_MetadataProxy->deleteLater();
    delete m_MetadataProxy;
    m_MetadataProxy = nullptr;
}

void MetadataTree::Setup()
{
    setAlternatingRowColors(true);

    m_MetadataProxy->setSourceModel(m_Metadata);
    setModel(m_MetadataProxy);

    setSortingEnabled(true);
}

VOID_NAMESPACE_CLOSE
