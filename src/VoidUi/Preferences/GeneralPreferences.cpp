// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "GeneralPreferences.h"
#include "Preferences.h"

VOID_NAMESPACE_OPEN

GeneralPreferences::GeneralPreferences(QWidget* parent)
    : BasicPreference(parent)
{
    /* Build UI */
    Build();

    /* Setup Default values */
    Setup();
}

GeneralPreferences::~GeneralPreferences()
{
}

void GeneralPreferences::Reset()
{
    int index = VoidPreferences::Instance().GetSetting(Settings::UndoQueueSize).toInt();
    m_UndoBox->setCurrentIndex(index);
}

void GeneralPreferences::Save()
{
    /* Get and save the value of the Undo Queue size */
    VoidPreferences::Instance().Set(Settings::UndoQueueSize, QVariant(m_UndoBox->currentIndex()));
}

void GeneralPreferences::Build()
{
    /* The internal layout */
    m_Layout = new QGridLayout(this);

    /* Add Preferences */
    m_UndoDescription = new QLabel("This setting describes the how many commands can be stored in the Undo History.\n\n\
 Once this limit is reached, the oldest commands will be discarded to make room for new ones.\n\
 Lower Values: Reduces memory usage but limit how far back user can undo.\n\
 Higher Values: More undo history but may consume more memory.\n");

    m_UndoLabel = new QLabel("Undo Queue Size");
    m_UndoBox = new QComboBox;

    /* Add to the layout */
    m_Layout->addWidget(m_UndoDescription, 0, 0, 1, 3);
    m_Layout->addWidget(m_UndoLabel, 1, 0);
    m_Layout->addWidget(m_UndoBox, 1, 1);

    /* Spacer */
    m_Layout->setRowStretch(2, 1);
}

void GeneralPreferences::Setup()
{
    /* Default values */
    m_UndoBox->addItems({"50", "100", "200", "Unlimited"});
    m_UndoBox->setCurrentIndex(0);
}

VOID_NAMESPACE_CLOSE
