// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QListWidgetItem>

/* Internal */
#include "CachePreferences.h"
#include "GeneralPreferences.h"
#include "MediaViewPreferences.h"
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

    setWindowTitle("VOID Preferences");
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
    /* Add General Preferences */
    GeneralPreferences* m_GeneralPrefs = new GeneralPreferences(this);

    /* Reset the preferences from the saved config */
    m_GeneralPrefs->Reset();

    /* Add the item and the widget to the List and Stack */
    m_SettingsList->addItem(ListViewItem("General", ":resources/icons/icon_setting.png"));
    m_SettingsStacked->addWidget(m_GeneralPrefs);

    /* Add Player Preferences */
    PlayerPreferences* m_PlayerPrefs = new PlayerPreferences(this);

    /* Reset the preferences from the saved config */
    m_PlayerPrefs->Reset();

    /* Add the item and the widget to the List and Stack */
    m_SettingsList->addItem(ListViewItem("Player", ":resources/icons/icon_video_player.png"));
    m_SettingsStacked->addWidget(m_PlayerPrefs);

    /* Add Cache Preferences */
    CachePreferences* m_CachePreferences = new CachePreferences(this);

    /* Reset the preferences from the saved config */
    m_CachePreferences->Reset();

    /* Add the item and the widget to the List and Stack */
    m_SettingsList->addItem(ListViewItem("Caching", ":resources/icons/icon_cache.png"));
    m_SettingsStacked->addWidget(m_CachePreferences);

    /* Add Media View Preferences */
    MediaViewPreferences* m_MediaViewPrefs = new MediaViewPreferences(this);

    /* Reset the preferences from the saved config */
    m_MediaViewPrefs->Reset();

    /* Add the item and the widget to the List and Stack */
    m_SettingsList->addItem(ListViewItem("Media View", ":resources/icons/icon_media_view.png"));
    m_SettingsStacked->addWidget(m_MediaViewPrefs);
}

QListWidgetItem* VoidPreferencesWidget::ListViewItem(const std::string& title, const std::string& icon) const
{
    QListWidgetItem* item = new QListWidgetItem(title.c_str(), m_SettingsList);
    item->setSizeHint(QSize(item->sizeHint().width(), 40));
    item->setIcon(QIcon(icon.c_str()));

    return item;
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
