// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_PREFERENCES_UI_H
#define _VOID_PREFERENCES_UI_H

/* Qt */
#include <QDialog>
#include <QLayout>
#include <QListWidget>
#include <QPushButton>
#include <QStackedWidget>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class VoidPreferencesWidget : public QDialog
{
    Q_OBJECT

public:
    VoidPreferencesWidget(QWidget* parent = nullptr);
    ~VoidPreferencesWidget();

    virtual inline QSize sizeHint() const override { return QSize(600, 550); };

private: /* Members */
    QVBoxLayout* m_layout;
    QHBoxLayout* m_PreferencesLayout;
    QHBoxLayout* m_ButtonsLayout;

    /* List and Stack Widget */
    QListWidget* m_SettingsList;
    QStackedWidget* m_SettingsStacked;

    /* Basic Buttons */
    QPushButton* m_OkButton;
    QPushButton* m_ApplyButton;
    QPushButton* m_CancelButton;

private: /* Methods */
    /* Builds the UI layout */
    void Build();
    /* Adds all internal settings */
    void AddSettings();
    /* Connect Signals */
    void Connect();

    /* Save the settings for the current index */
    void SaveCurrentSettings();
    void SaveAndExit();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PREFERENCES_UI_H
