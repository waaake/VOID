#ifndef _VOID_ANNOTATION_CONTROLLER_H
#define _VOID_ANNOTATION_CONTROLLER_H

/* Qt */
#include <QLayout>
#include <QPushButton>
#include <QButtonGroup>
#include <QWidget>

/* Internal */
#include "QDefinition.h"
#include "QExtensions/PushButton.h"
#include "QExtensions/Slider.h"

VOID_NAMESPACE_OPEN

class AnnotationsController : public QWidget
{
    Q_OBJECT
public:
    AnnotationsController(QWidget* parent = nullptr);
    ~AnnotationsController();

signals:
    void cleared();
    /**
     * Gets emitted when a control has been selected
     * this indicates the Renderer to change the DrawType
     * e.g. when brush is selected and drawn with
     * later eraser was selected to update the DrawType on the Renderer
     */
    void controlChanged(const int);

    void colorChanged(const QColor&);
    void brushSizeChanged(const int);

private: /* Members */
    QVBoxLayout* m_Layout;

    /* Button Group Allowing only one selection at a time */
    QButtonGroup* m_AnnotationGroup;
   
    /* Sliders */
    FramelessSlider* m_SizeSlider;
    
    /* Buttons */
    QPushButton* m_PointerButton;
    QPushButton* m_BrushButton;
    QPushButton* m_TextButton;
    QPushButton* m_EraserButton;
    QPushButton* m_ClearButton;
    ColorSelectionButton* m_ColorButton;
    QPushButton* m_SizeAdjuster;

private: /* Methods */
    /**
     * Builds the Overall Layout of the Controller
     */
    void Build();

    /**
     * Connects Signals
     */
    void Connect();

    /**
     * Setup the UI defaults
     */
    void Setup();

    /**
     * Shows the Size Slider
     */
    void OpenSizeSelector();

};

VOID_NAMESPACE_CLOSE

#endif // _VOID_ANNOTATION_CONTROLLER_H
