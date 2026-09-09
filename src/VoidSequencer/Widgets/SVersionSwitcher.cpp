// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "SVersionSwitcher.h"
#include "VoidMediaPlayer/Media/Delegates/ListDelegate.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

SVersionSwitcher::SVersionSwitcher(QWidget* parent)
    : TranslucentDialog(parent)
{
    Build();
    setFixedWidth(400);
}

SVersionSwitcher::~SVersionSwitcher()
{
    m_Layout->deleteLater();
    delete m_Layout;
    m_Layout = nullptr;

    m_View->deleteLater();
    delete m_View;
    m_View = nullptr;
}

void SVersionSwitcher::SetElementName(const QString& name)
{
    m_View->SetElementName(name);
    int rows = m_View->RowCount();
    setMaximumHeight((rows > 6 ? 360 : (rows * 60)) + 24);
}

void SVersionSwitcher::Build()
{
    m_Layout = new QVBoxLayout(this);

    m_View = new VersionView(this);
    m_View->setItemDelegate(new MediaItemDelegate(m_View));

    m_Layout->addWidget(m_View);
}

VOID_NAMESPACE_CLOSE
