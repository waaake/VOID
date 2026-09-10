// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "SVersionSwitcher.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

SVersionSwitcher::SVersionSwitcher(SequencerContext* context, QWidget* parent)
    : TranslucentDialog(parent)
    , m_Context(context)
{
    Build();
    connect(m_View, &VersionView::versionChanged, this, &SVersionSwitcher::ResetMedia);
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

void SVersionSwitcher::Exec(const SharedTrackItem& item, const QPoint& position)
{
    m_Item = item;
    SetElementName(item->Tokens().name.c_str());
    move(position);
    exec();
}

void SVersionSwitcher::Build()
{
    m_Layout = new QVBoxLayout(this);

    m_View = new VersionView(this);
    m_Layout->addWidget(m_View);
}

void SVersionSwitcher::SetElementName(const QString& name)
{
    m_View->SetElementName(name);
    int rows = m_View->RowCount();
    setMaximumHeight((rows > 6 ? 360 : (rows * 60)) + 24);
}

void SVersionSwitcher::ResetMedia(const QModelIndex& index)
{
    m_Context->Controller()->ResetMedia(m_Item, index);
}

VOID_NAMESPACE_CLOSE
