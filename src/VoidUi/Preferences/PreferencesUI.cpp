// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QListWidgetItem>

/* Internal */
#include "PreferencesUI.h"
#include "PlayerPreferences.h"

VOID_NAMESPACE_OPEN

VoidPreferencesWidget::VoidPreferencesWidget(QWidget* parent)
    : QDialog(parent)
{
    /* Build the Layout */
    Build();

    /* Add Settings */
    AddSettings();

    /* Connect Signals */
    Connect();

    /* Set Current Selection Index */
    m_SettingsList->setCurrentRow(0);
}

VoidPreferencesWidget::~VoidPreferencesWidget()
{
}

void VoidPreferencesWidget::Build()
{
    /* Main Layout */
    m_layout = new QVBoxLayout(this);

    /* Internals */
    m_PreferencesLayout = new QHBoxLayout;

    m_SettingsList = new QListWidget;
    m_SettingsList->setFixedWidth(180);
    m_SettingsStacked = new QStackedWidget;

    m_PreferencesLayout->addWidget(m_SettingsList);
    m_PreferencesLayout->addWidget(m_SettingsStacked);

    /* Buttons */
    m_OkButton = new QPushButton("Ok");
    m_ApplyButton = new QPushButton("Apply");
    m_CancelButton = new QPushButton("Cancel");

    m_ButtonsLayout = new QHBoxLayout;
    
    /* Add to the layout */
    m_ButtonsLayout->addStretch(1);
    m_ButtonsLayout->addWidget(m_OkButton);
    m_ButtonsLayout->addWidget(m_CancelButton);
    m_ButtonsLayout->addWidget(m_ApplyButton);

    /* Add to the main layout */
    m_layout->addLayout(m_PreferencesLayout);
    m_layout->addLayout(m_ButtonsLayout);
}

void VoidPreferencesWidget::AddSettings()
{
    /* Add Player Preferences */
    PlayerPreferences* m_PlayerPrefs = new PlayerPreferences(this);

    /* Reset the preferences from the saved config */
    m_PlayerPrefs->Reset();

    QListWidgetItem* item = new QListWidgetItem("Player Preferences", m_SettingsList);
    item->setSizeHint(QSize(item->sizeHint().width(), 40));
    item->setIcon(QIcon(":resources/icons/icon_video_player.png"));

    /* Add the item and the widget to the List and Stack */
    m_SettingsList->addItem(item);
    m_SettingsStacked->addWidget(m_PlayerPrefs);
}

void VoidPreferencesWidget::SaveCurrentSettings()
{
    /* Get the widget at the current index */
    BasicPreference* w = static_cast<BasicPreference*>(m_SettingsStacked->currentWidget());
    /* Save the settings from the current widget */
    w->Save();
}

void VoidPreferencesWidget::SaveAndExit()
{
    /* Save the current index's settings */
    SaveCurrentSettings();

    /* And Exit */
    close();
}

void VoidPreferencesWidget::Connect()
{
    /* List - row changed -> Stacked - set current index */
    connect(m_SettingsList, &QListWidget::currentRowChanged, m_SettingsStacked, &QStackedWidget::setCurrentIndex);

    /* Buttons */
    connect(m_CancelButton, &QPushButton::clicked, this, &QWidget::close);
    connect(m_OkButton, &QPushButton::clicked, this, &VoidPreferencesWidget::SaveAndExit);
    connect(m_ApplyButton, &QPushButton::clicked, this, &VoidPreferencesWidget::SaveCurrentSettings);
}

VOID_NAMESPACE_CLOSE
