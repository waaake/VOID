// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_COLOR_CONTROLLER_H
#define _VOID_COLOR_CONTROLLER_H

/* Qt */
#include <QObject>

/* Internal */
#include "ControlScroller.h"
#include "Definition.h"
#include "VoidQExtensions/ComboBox.h"

VOID_NAMESPACE_OPEN

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

VOID_NAMESPACE_CLOSE

#endif // _VOID_COLOR_CONTROLLER_H
