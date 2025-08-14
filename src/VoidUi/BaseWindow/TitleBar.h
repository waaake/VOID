// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_TITLE_BAR_H
#define _VOID_TITLE_BAR_H

/* Qt */
#include <QLabel>
#include <QLayout>
#include <QMenuBar>
#include <QToolButton>

/* Internal */
#include "Definition.h"
#include "VoidUi/QExtensions/PushButton.h"

VOID_NAMESPACE_OPEN

class VoidTitleBar : public QWidget
{
    Q_OBJECT

public:
    VoidTitleBar(QWidget* parent = nullptr);
    virtual ~VoidTitleBar();

    QMenuBar* MenuBar() const { return m_MenuBar; }

signals:
    void requestMinimize();
    void requestMaximizeRestore();
    void requestClose();

protected:
    void paintEvent(QPaintEvent* event) override;

private: /* Members */
    QWidget* m_LeftCorner;
    QHBoxLayout* m_LeftLayout;
    QLabel* m_VoidLabel;

    /* Right side box -- minimize | maximize | close */
    QHBoxLayout* m_RightLayout;
    QToolButton* m_MinimizeButton;
    QToolButton* m_MaximizeButton;
    CloseButton* m_CloseButton;

    QHBoxLayout* m_MenuLayout;

    QPoint m_DragPosition;
    bool m_Pressed;

    QMenuBar* m_MenuBar;

private: /* Methods */
    void Build();

    /* Connect Signals for the title bar Window Options */
    void Connect();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_TITLE_BAR_H
