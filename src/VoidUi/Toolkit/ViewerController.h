// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VIEWER_CONTROLLER_H
#define _VIEWER_CONTROLLER_H

/* Qt */
#include <QLayout>
#include <QSlider>
#include <QWidget>
#include <QPushButton>

/* Internal */
#include "Definition.h"
#include "VoidUi/QExtensions/Dialog.h"

VOID_NAMESPACE_OPEN

enum class ViewerControl
{
    None,
    GridControl,
    OnionSkinControl
};

class ViewerController : public QWidget
{
    Q_OBJECT
public:
    ViewerController(QWidget* parent = nullptr);
    ~ViewerController();

    void SetControl(const ViewerControl& control);
    ViewerControl Control() const { return m_ViewerControl; }

private: /* Members */
    QLayout* m_Layout;
    QPushButton* m_ControlOption;

    ViewerControl m_ViewerControl;

private: /* Methods */
    void Build();
    void Connect();
    void Setup();
    QIcon Icon(const ViewerControl& control);
};

class OnionSkinController : public TranslucentDialog
{
public:
    OnionSkinController(QWidget* parent = nullptr);
    ~OnionSkinController();

private: /* Members */
    QHBoxLayout* m_Layout;
    QSlider* m_PeelSlider;

private: /* Methods */
    void Build();
    void Setup();
};

VOID_NAMESPACE_CLOSE

#endif // _VIEWER_CONTROLLER_H
