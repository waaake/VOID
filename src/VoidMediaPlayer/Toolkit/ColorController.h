// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_COLOR_CONTROLLER_H
#define _VOID_COLOR_CONTROLLER_H

/* Qt */
#include <QLayout>
#include <QObject>
#include <QWidget>

/* Internal */
#include "ControlScroller.h"
#include "Definition.h"
#include "VoidQExtensions/ComboBox.h"
#include "VoidQExtensions/PushButton.h"

VOID_NAMESPACE_OPEN

class ColorEngine;

class ColorController : public ControlCombo
{
    Q_OBJECT
public:
    ColorController(QWidget* parent = nullptr);
    
    /**
     * Current Display Settings for the Controller
     */
    inline void SetDisplay(const std::string& name) { setCurrentText(name.c_str()); }
    inline std::string Display() const { return currentText().toStdString(); }

signals:
    void colorDisplayChanged(const std::string&);

private: /* Methods */
    void Setup();
};

class ColorController2 : public QWidget
{
    Q_OBJECT
public:
    explicit ColorController2(QWidget* parent = nullptr);
    ~ColorController2();

    void Setup(ColorEngine* engine);

signals:
    void editColorSettingsRequested(ColorEngine* engine);

private:
    QHBoxLayout* m_Layout;
    ControlCombo* m_ViewController;
    QToolButton* m_SettingsButton;
    ColorEngine* m_Engine;

private:
    void Build();
    void Reset();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_COLOR_CONTROLLER_H
