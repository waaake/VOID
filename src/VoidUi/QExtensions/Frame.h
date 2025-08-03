// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_Q_EXT_FRAME_H
#define _VOID_Q_EXT_FRAME_H

/* STD */
#include <vector>

/* Qt */
#include <QAction>
#include <QActionGroup>
#include <QFrame>
#include <QPushButton>
#include <QMenu>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class SplitSectionSelector : public QPushButton
{
    Q_OBJECT

public:
    SplitSectionSelector(QWidget* parent = nullptr);
    ~SplitSectionSelector();

    /**
     * Adds the Main items to the ComboBox over which the ComboBox
     * would allow selection directly and show up in the TextArea of the Box
     */
    void AddPrimaryItems(const QStringList& texts);

    /**
     * Adds a custom Define non-selectable Separator
     */
    void AddSeparator();

    /**
     * Adds the items which are to be shown with a RadioBox next to it
     * allowing selection of one of the items to show up in the popup and indicate
     * something from that list is currently active
     */
    void AddRadioItems(const QStringList& texts);

signals:
    /**
     * Emitted when any of the Primary Items are selected from the Menu
     */
    void primaryIndexChanged(const int);
    /**
     * Emitted when any of the Radio item in the Menu is selected
     * emits the index of the Item in the Radio List
     */
    void radioIndexChanged(const int);

protected: /* Members */
    std::vector<QAction*> m_RadioActions;

private: /* Members */
    QMenu* m_Menu;
    QActionGroup* m_RadioGroup;

private: /* Methods */
    void PrimaryItemSelected(const QString& text, const int index);
    void RadioItemSelected(const QString& text, const int index);

};

VOID_NAMESPACE_CLOSE

#endif // _VOID_Q_EXT_FRAME_H
