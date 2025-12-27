// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "GeneralPreferences.h"
#include "Preferences.h"
#include "VoidCore/Logging.h"

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

    index = VoidPreferences::Instance().GetSetting(Settings::ColorStyle).toInt();
    m_ColorStyleBox->setCurrentIndex(index);

    m_StartupPopupCheck->setChecked(VoidPreferences::Instance().GetSetting(Settings::DontShowStartup).toBool());
}

void GeneralPreferences::Save()
{
    /* Get and save the value of the Undo Queue size */
    VoidPreferences::Instance().Set(Settings::UndoQueueSize, QVariant(m_UndoBox->currentIndex()));
    VoidPreferences::Instance().Set(Settings::ColorStyle, QVariant(m_ColorStyleBox->currentIndex()));
    VoidPreferences::Instance().Set(Settings::DontShowStartup, QVariant(m_StartupPopupCheck->isChecked()));
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

    m_ColorStyleDescription = new QLabel("Personalize the look and feel of VOID player by selecting a custom color theme.\n\
 This setting allows you to change the interface to match your preferences.\n\
 Changes will take effect the next time the application is restarted.");

    m_ColorStyleLabel = new QLabel("Color Style");
    m_ColorStyleBox = new QComboBox;

    m_StartupPopupDescription = new QLabel("Show a popup when the Application starts. The popup allows to choose from the recent Projects.");
    m_StartupPopupLabel = new QLabel("Don't show Statup Dialog");
    m_StartupPopupCheck = new QCheckBox();

    /* Add to the layout */
    m_Layout->addWidget(m_UndoDescription, 0, 0, 1, 3);
    m_Layout->addWidget(m_UndoLabel, 1, 0);
    m_Layout->addWidget(m_UndoBox, 1, 1);

    m_Layout->addItem(new QSpacerItem(10, 20), 2, 3);

    m_Layout->addWidget(m_ColorStyleDescription, 3, 0, 1, 3);
    m_Layout->addWidget(m_ColorStyleLabel, 4, 0);
    m_Layout->addWidget(m_ColorStyleBox, 4, 1);

    m_Layout->addItem(new QSpacerItem(10, 20), 5, 3);

    m_Layout->addWidget(m_StartupPopupDescription, 6, 0, 1, 3);
    m_Layout->addWidget(m_StartupPopupLabel, 7, 0);
    m_Layout->addWidget(m_StartupPopupCheck, 7, 1);

    /* Spacer */
    m_Layout->setRowStretch(8, 1);
}

void GeneralPreferences::Setup()
{
    /* Default values */
    m_UndoBox->addItems({"50", "100", "200", "Unlimited"});
    m_UndoBox->setCurrentIndex(0);

    m_ColorStyleBox->addItems({"Default Dark", "Night Blue", "Sakura Pink", "Obsidian", "Default White", "Void Gray"});
    m_ColorStyleBox->setCurrentIndex(0);
}

VOID_NAMESPACE_CLOSE
