// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "MediaSearchBar.h"
#include "VoidIconForge/IconForge.h"

VOID_NAMESPACE_OPEN

MediaSearchBar::MediaSearchBar(QWidget* parent)
    : QLineEdit(parent)
{
    /* Setup UI */
    Setup();

    /* Connect Signals */
    Connect();
}

MediaSearchBar::~MediaSearchBar()
{
    m_ClearAction->deleteLater();
    delete m_ClearAction;
    m_ClearAction = nullptr;
}

void MediaSearchBar::Setup()
{
    setPlaceholderText("Search");

    m_ClearAction = new QAction(IconForge::GetIcon(IconType::icon_close, _DARK_COLOR(QPalette::Text, 100)), "", this);
    m_ClearAction->setToolTip("Clears Search field");

    addAction(m_ClearAction, QLineEdit::TrailingPosition);
    m_ClearAction->setVisible(false);

    setFocusPolicy(Qt::ClickFocus);
}

void MediaSearchBar::Connect()
{
    connect(this, &QLineEdit::textChanged, this, &MediaSearchBar::TextChanged);
    connect(m_ClearAction, &QAction::triggered, this, &QLineEdit::clear);
}

void MediaSearchBar::TextChanged(const QString& text)
{
    m_ClearAction->setVisible(!text.isEmpty());
    emit typed(text.toStdString());
}

VOID_NAMESPACE_CLOSE
