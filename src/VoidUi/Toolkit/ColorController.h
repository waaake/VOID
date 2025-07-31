#ifndef _VOID_COLOR_CONTROLLER_H
#define _VOID_COLOR_CONTROLLER_H

/* Qt */
#include <QObject>

/* Internal */
#include "ControlScroller.h"
#include "Definition.h"
// #include ""

VOID_NAMESPACE_OPEN

class ColorController : public ControlCombo
{
    Q_OBJECT

public:
    ColorController(QWidget* parent = nullptr);
    ~ColorController();
    
    /**
     * Current Display Settings for the Controller
     */
    inline void SetDisplay(const std::string& name) { setCurrentText(name.c_str()); }
    inline std::string Display() const { return currentText().toStdString(); }

signals:
    /* Emitted when the current text on the Controller is changed */
    void colorDisplayChanged(const std::string&);

private: /* Methods */
    /**
     * Initializes the Color Processor for VOID
     * Sets up the Default config to be used
     */
    void InitColorProcessor();

    /**
     * Setup the current Display values on the Controller
     */
    void Setup();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_COLOR_CONTROLLER_H