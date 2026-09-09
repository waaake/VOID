// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "VersionView.h"

VOID_NAMESPACE_OPEN

VersionView::VersionView(QWidget* parent)
    : QListView(parent)
{
    m_Proxy = new MediaVersionProxyModel(this);
    setModel(m_Proxy);
}

VersionView::~VersionView()
{
    m_Proxy->deleteLater();
    delete m_Proxy;
    m_Proxy = nullptr;
}

void VersionView::ResetModel(EntityModel* model)
{
    m_Proxy->setSourceModel(model);
}

void VersionView::SetElementName(const QString& name)
{
    m_Proxy->SetElementName(name);
}

VOID_NAMESPACE_CLOSE
