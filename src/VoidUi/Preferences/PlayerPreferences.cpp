/* Internal */
#include "PlayerPreferences.h"
#include "Preferences.h"

VOID_NAMESPACE_OPEN

PlayerPreferences::PlayerPreferences(QWidget* parent)
    : BasicPreference(parent)
{
    /* Build UI */
    Build();

    /* Setup Default values */
    Setup();
}

PlayerPreferences::~PlayerPreferences()
{
}

void PlayerPreferences::Reset()
{
    int index = VoidPreferences::Instance().GetSetting(Settings::MissingFramesHandler).toInt();
    m_MissingFramesBox->setCurrentIndex(index);
}

void PlayerPreferences::Save()
{
    /* Get and save the value of the Missing frames handler */
    VoidPreferences::Instance().Set(Settings::MissingFramesHandler, QVariant(m_MissingFramesBox->currentIndex()));
}

void PlayerPreferences::Build()
{
    /* The internal layout */
    m_Layout = new QGridLayout(this);

    /* Add Preferences */
    m_MissingFramesDescription = new QLabel("This setting describes the behaviour of the player on how to handle missing frames.\n\n\
 Error: Shows an error on the viewer when a missing frame is player.\n\
 Black Frame: Player shows a Black frame for the missing frame.\n\
 Nearest: Player sets the last nearest frame for a missing frame.\n");

    m_MissingFramesLabel = new QLabel("Handle Missing Frames as");
    m_MissingFramesBox = new QComboBox;

    /* Add to the layout */
    m_Layout->addWidget(m_MissingFramesDescription, 0, 0, 1, 3);
    m_Layout->addWidget(m_MissingFramesLabel, 1, 0);
    m_Layout->addWidget(m_MissingFramesBox, 1, 1);

    /* Spacer */
    m_Layout->setRowStretch(2, 1);
}

void PlayerPreferences::Setup()
{
    /* Default values */
    m_MissingFramesBox->addItems({"Error", "Black Frame", "Nearest"});
    m_MissingFramesBox->setCurrentIndex(0);
}

VOID_NAMESPACE_CLOSE
