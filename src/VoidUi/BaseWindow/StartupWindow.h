// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_STARTUP_WINDOW_H
#define _VOID_STARTUP_WINDOW_H

/* Qt */
#include <QCheckBox>
#include <QDialog>
#include <QLabel>
#include <QLayout>
#include <QListWidget>
#include <QPushButton>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class StartupWindow : public QDialog
{
    Q_OBJECT
public:
    StartupWindow(QWidget* parent = nullptr);
    ~StartupWindow();

    inline virtual QSize sizeHint() const override { return QSize(650, 350); }

private: /* Members */
    QPushButton* m_NewProjectBtn;
    QPushButton* m_OpenProjectBtn;
    QPushButton* m_LoadBtn;
    QPushButton* m_CloseBtn;

    QCheckBox* m_DontShowCheck;
    QListWidget* m_ProjectsLister;

    QVBoxLayout* m_Layout;
    QHBoxLayout* m_InternalSplitLayout;
    QVBoxLayout* m_SideButtonsLayout;
    QHBoxLayout* m_BottomButtonsLayout;

private: /* Methods */
    void Build();
    void Connect();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_STARTUP_WINDOW_H