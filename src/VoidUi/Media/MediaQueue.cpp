// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QKeyEvent>

/* Internal */
#include "MediaQueue.h"
#include "VoidUi/Engine/IconForge.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

MediaQueue::MediaQueue(QWidget* parent)
    : QWidget(parent)
{
    Build();
    Setup();
    Connect();
}

MediaQueue::~MediaQueue()
{
    m_View->deleteLater();
    delete m_View;
    m_View = nullptr;

    m_Layout->deleteLater();
    delete m_Layout;
    m_Layout = nullptr;
}

void MediaQueue::Set(Playlist* playlist)
{
    m_Playlist = playlist;
    m_Playlist ? m_View->Set(playlist) : m_View->Clear();
}

bool MediaQueue::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::ShortcutOverride)
    {
        QKeyEvent* k = static_cast<QKeyEvent*>(event);
        #ifdef _VOID_PLATFORM_APPLE
        if (k->key() == Qt::Key_Backspace) m_Playlist->RemoveMedia(m_View->currentIndex());
        #else
        if (k->key() == Qt::Key_Delete) m_Playlist->RemoveMedia(m_View->currentIndex());
        #endif
    }
    return true;
}

void MediaQueue::Build()
{
    m_Layout = new QVBoxLayout(this);

    QHBoxLayout* buttonLayout = new QHBoxLayout;

    m_MoveUpArrow = new QPushButton;
    m_MoveDownArrow = new QPushButton;

    buttonLayout->addWidget(m_MoveDownArrow);
    buttonLayout->addWidget(m_MoveUpArrow);
    buttonLayout->addStretch(1);

    m_View = new QueueView(this);
    m_Layout->addLayout(buttonLayout);
    m_Layout->addWidget(m_View);

    auto margins = m_Layout->contentsMargins();
    m_Layout->setContentsMargins(2, margins.top(), 2, 2);
}

void MediaQueue::Setup()
{
    m_MoveDownArrow->setFixedWidth(25);
    m_MoveUpArrow->setFixedWidth(25);

    m_MoveDownArrow->setIcon(IconForge::GetIcon(IconType::icon_expand_circle_down, _DARK_COLOR(QPalette::Text, 100)));
    m_MoveUpArrow->setIcon(IconForge::GetIcon(IconType::icon_expand_circle_up, _DARK_COLOR(QPalette::Text, 100)));

    installEventFilter(this);
}

void MediaQueue::Connect()
{
    connect(m_MoveDownArrow, &QPushButton::clicked, this, [&]() -> void
    {
        QModelIndex index = m_Playlist->ShiftIndexDown(m_View->currentIndex());
        if (index.isValid())
            m_View->setCurrentIndex(index);
    });
    connect(m_MoveUpArrow, &QPushButton::clicked, this, [&]() -> void
    {
        QModelIndex index = m_Playlist->ShiftIndexUp(m_View->currentIndex());
        if (index.isValid())
            m_View->setCurrentIndex(index);
    });
}

VOID_NAMESPACE_CLOSE
