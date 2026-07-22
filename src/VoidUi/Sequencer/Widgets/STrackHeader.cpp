// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>

/* Internal */
#include "STrackHeader.h"
#include "VoidUi/Engine/IconForge.h"

VOID_NAMESPACE_OPEN

constexpr int margin = 8;
constexpr int iconSize = 18;
constexpr int spacing = 10;
constexpr int nameWidth = 90;

STrackHeader::STrackHeader(const SharedPlaybackTrack& track, SequencerContext* context, QWidget* parent)
    : QWidget(parent)
    , m_Track(track)
    , m_Context(context)
{
    setMouseTracking(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(m_Context->SelectionModel(), &SSelectionModel::trackSelectionChanged, this, static_cast<void (STrackHeader::*)(void)>(&STrackHeader::update));
    connect(m_Track.get(), &PlaybackTrack::updated, this, [this]() -> void { update(); });
}

void STrackHeader::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(
        rect(),
        m_Context->SelectionModel()->IsSelected(m_Track)
        ? palette().color(QPalette::Highlight).darker(150)
        : palette().color(QPalette::Dark)
    );

    painter.setPen(palette().color(QPalette::Text));
    painter.drawText(NameRect(), Qt::AlignVCenter | Qt::AlignRight, m_Track->Name().c_str());

    painter.drawPixmap(
        StateRect().topLeft(),
        m_Track->Enabled()
        ? IconForge::GetPixmap(IconType::icon_visible, _DARK_COLOR(QPalette::Text, 100), 14)
        : IconForge::GetPixmap(IconType::icon_visible_off, _DARK_COLOR(QPalette::Highlight, 100), 14)
    );

    painter.drawPixmap(
        LockRect().topLeft(),
        m_Track->Locked()
        ? IconForge::GetPixmap(IconType::icon_lock, _DARK_COLOR(QPalette::Highlight, 100), 14)
        : IconForge::GetPixmap(IconType::icon_lock_open, _DARK_COLOR(QPalette::Text, 100), 14)
    );
}

void STrackHeader::mousePressEvent(QMouseEvent* event)
{
    QWidget::mousePressEvent(event);
    if (event->button() == Qt::LeftButton)
    {
        if (LockRect().contains(event->pos()))
        {
            m_Context->Controller()->ToggleTrackLock(m_Track);
        }
        else if (StateRect().contains(event->pos()))
        {
            m_Context->Controller()->ToggleTrackState(m_Track);
        }
        else if (event->modifiers() & Qt::ControlModifier)
        {
            m_Context->SelectionModel()->Toggle(m_Track);
        }
        else
        {   
            m_Context->SelectionModel()->Clear();
            m_Context->SelectionModel()->Select(m_Track);
        }
    }
}

void STrackHeader::mouseMoveEvent(QMouseEvent* event)
{
    QWidget::mouseMoveEvent(event);
}

void STrackHeader::mouseReleaseEvent(QMouseEvent* event)
{
    QWidget::mouseReleaseEvent(event);
}

void STrackHeader::resizeEvent(QResizeEvent* event)
{
    m_LockRect = QRect(margin, (height() - iconSize) * 0.5, iconSize, iconSize);
    m_StateRect = QRect(m_LockRect.right() + spacing, (height() - iconSize) * 0.5, iconSize, iconSize);
    m_NameRect = QRect(m_StateRect.right() + spacing, 0, width() - (m_StateRect.right() + spacing + margin), height());

    QWidget::resizeEvent(event);
}

VOID_NAMESPACE_CLOSE
