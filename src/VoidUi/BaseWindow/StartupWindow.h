// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_STARTUP_WINDOW_H
#define _VOID_STARTUP_WINDOW_H

/* Qt */
#include <QCheckBox>
#include <QDialog>
#include <QLabel>
#include <QLayout>
#include <QListView>
#include <QPushButton>
#include <QStyledItemDelegate>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Models/ProjectModel.h"

VOID_NAMESPACE_OPEN

class LogoWidget : public QWidget
{
public:
    LogoWidget(QWidget* parent = nullptr);
protected:
    void paintEvent(QPaintEvent* event);
};

class StartupWindow : public QDialog
{
    Q_OBJECT
public:
    StartupWindow(QWidget* parent = nullptr);
    ~StartupWindow();

    inline virtual QSize sizeHint() const override { return QSize(650, 350); }
    static void Exec(QWidget* parent = nullptr);

private: /* Members */
    QPushButton* m_NewProjectBtn;
    QPushButton* m_OpenProjectBtn;
    QPushButton* m_LoadBtn;
    QPushButton* m_CloseBtn;

    QCheckBox* m_DontShowCheck;
    QListView* m_ProjectsLister;

    QVBoxLayout* m_Layout;
    QHBoxLayout* m_InternalSplitLayout;
    QVBoxLayout* m_ProjectsLayout;
    QVBoxLayout* m_SideButtonsLayout;
    QHBoxLayout* m_BottomButtonsLayout;

    RecentProjectsModel* m_Projects;

private: /* Methods */
    void Build();
    void Connect();
    void Setup();
    void Populate();

    void LoadSelected();
    void Close();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_STARTUP_WINDOW_H