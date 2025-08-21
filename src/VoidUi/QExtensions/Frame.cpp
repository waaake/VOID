// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Frame.h"

VOID_NAMESPACE_OPEN

SplitSectionSelector::SplitSectionSelector(QWidget* parent)
    : QPushButton(parent)
{
    /* Setup Menu */
    m_Menu = new QMenu(this);

    /* Radio Group with only one selection at a time */
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

        /* Any Item selection would trigger a signal to invoke that the Primary Item has been selected */
        connect(action, &QAction::triggered, this, [=]() { PrimaryItemSelected(text, i); });

        /* Add to the Menu */
        m_Menu->addAction(action);

        /* Default Text on the Frame */
        if (!i) // will only run for the first index
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
        action->setActionGroup(m_RadioGroup);

        /* Any Item selection would trigger a signal to invoke that the Radio Item has been selected */
        connect(action, &QAction::triggered, this, [=]() { RadioItemSelected(text, i); });

        /* Add to the Menu */
        m_Menu->addAction(action);

        /* Store them in the Vector */
        m_RadioActions.push_back(action);
    }
}

void SplitSectionSelector::PrimaryItemSelected(const QString& text, const int index)
{
    /* The text would get displayed on the Frame */
    setText(text);

    /* And the index gets emitted */
    emit primaryIndexChanged(index);
}

void SplitSectionSelector::RadioItemSelected(const QString& text, const int index)
{
    /* Emit the index */
    emit radioIndexChanged(index);
}

VOID_NAMESPACE_CLOSE
