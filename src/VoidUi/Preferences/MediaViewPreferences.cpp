// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "MediaViewPreferences.h"
#include "Preferences.h"

VOID_NAMESPACE_OPEN

MediaViewPreferences::MediaViewPreferences(QWidget* parent)
    : BasicPreference(parent)
{
    /* Build UI */
    Build();

    /* Setup Default values */
    Setup();
}

MediaViewPreferences::~MediaViewPreferences()
{
}

void MediaViewPreferences::Reset()
{
    int index = VoidPreferences::Instance().GetSetting(Settings::MediaViewType).toInt();
    m_ViewBox->setCurrentIndex(index);
}

void MediaViewPreferences::Save()
{
    /* Get and save the value of the Missing frames handler */
    VoidPreferences::Instance().Set(Settings::MediaViewType, QVariant(m_ViewBox->currentIndex()));
}

void MediaViewPreferences::Build()
{
    /* The internal layout */
    m_Layout = new QGridLayout(this);

    /* Add Preferences */
    m_ViewDescription = new QLabel("This setting describes the default view on Media View.\n\n\
 List: Shows imported media items as list with few details.\n\
 Detailed List: Shows imported media items as list with thumbnail and details.\n\
 Thumbnail: Shows imported media items in a thumbnail style.\n");

    m_ViewLabel = new QLabel("Default View");
    m_ViewBox = new QComboBox;

    /* Add to the layout */
    m_Layout->addWidget(m_ViewDescription, 0, 0, 1, 3);
    m_Layout->addWidget(m_ViewLabel, 1, 0);
    m_Layout->addWidget(m_ViewBox, 1, 1);

    /* Spacer */
    m_Layout->setRowStretch(2, 1);
}

void MediaViewPreferences::Setup()
{
    /* Default values */
    m_ViewBox->addItems({"List", "Detailed List", "Thumbnail"});
    m_ViewBox->setCurrentIndex(0);
}

VOID_NAMESPACE_CLOSE
