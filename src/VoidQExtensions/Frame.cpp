// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>

/* Internal */
#include "Frame.h"

VOID_NAMESPACE_OPEN

/// SplitSectionSelector

SplitSectionSelector::SplitSectionSelector(QWidget* parent)
    : QPushButton(parent)
{
    m_Menu = new QMenu(this);
    m_RadioGroup = new QActionGroup(m_Menu);
    m_RadioGroup->setExclusive(true);

    setMenu(m_Menu);
    setFlat(true);
}

SplitSectionSelector::~SplitSectionSelector()
{
    /**
     * Remove Entities from the Vector
     * the pointer to that action will get removed internally when the menu is deleted
     */
    m_RadioActions.clear();
    m_Menu->deleteLater();
}

void SplitSectionSelector::AddPrimaryItems(const QStringList& texts)
{
    for (int i = 0; i < texts.size(); i++)
    {
        QString text = texts[i];
        QAction* action = new QAction(text, m_Menu);

        connect(action, &QAction::triggered, this, [=]() { PrimaryItemSelected(text, i); });
        m_Menu->addAction(action);
        if (i) continue;

        setText(text);
    }
}

void SplitSectionSelector::AddSeparator()
{
    m_Menu->addSeparator();
}

void SplitSectionSelector::AddRadioItems(const QStringList& texts)
{
    for (int i = 0; i < texts.size(); i++)
    {
        QString text = texts[i];
        QAction* action = new QAction(text, m_Menu);
        action->setCheckable(true);
        action->setObjectName(QString::number(i));
        action->setActionGroup(m_RadioGroup);

        connect(action, &QAction::triggered, this, [=]() { RadioItemSelected(text, i); });
        m_Menu->addAction(action);
        m_RadioActions.push_back(action);
    }
}

void SplitSectionSelector::CycleRadioActionsForwards()
{
    auto action = m_RadioGroup->checkedAction();
    int next = (action->objectName().toInt() + 1) % static_cast<int>(m_RadioActions.size());

    m_RadioActions[next]->trigger();
}

void SplitSectionSelector::CycleRadioActionsBackwards()
{
    auto action = m_RadioGroup->checkedAction();
    int previous = std::abs((action->objectName().toInt() - 1) % static_cast<int>(m_RadioActions.size()));

    m_RadioActions[previous]->trigger();
}

void SplitSectionSelector::PrimaryItemSelected(const QString& text, const int index)
{
    setText(text);
    emit primaryIndexChanged(index);
}

void SplitSectionSelector::RadioItemSelected(const QString& text, const int index)
{
    emit radioIndexChanged(index);
}

/// VLine

VLine::VLine(QWidget* parent)
    : QWidget(parent)
{
    setFixedWidth(2);
}

void VLine::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(QPen(palette().color(QPalette::Dark).darker(200), 2));
    painter.drawLine(width() - 1, 0, width() - 1, height());
    painter.setPen(QPen(palette().color(QPalette::Window).lighter(180), 2));
    painter.drawLine(width(), 0, width(), height());
}

/// BaseWidget

void BaseWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.fillRect(rect(), palette().color(QPalette::Base));
}

VOID_NAMESPACE_CLOSE
