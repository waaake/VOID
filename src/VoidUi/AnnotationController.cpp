/* Internal */
#include "AnnotationController.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

AnnotationsController::AnnotationsController(QWidget* parent)
    : QWidget(parent)
{
    Build();

    /* Connect Signals */
    Connect();

    /* Setup Defaults */
    Setup();

    /* This controller has a fixed width */
    setFixedWidth(40);
}

AnnotationsController::~AnnotationsController()
{
    m_PointerButton->deleteLater();
    m_BrushButton->deleteLater();
    m_EraserButton->deleteLater();
    m_TextButton->deleteLater();
    m_ClearButton->deleteLater();
    m_ColorButton->deleteLater();
    m_SizeAdjuster->deleteLater();

    m_SizeSlider->deleteLater();

    m_AnnotationGroup->deleteLater();
    m_Layout->deleteLater();
}

void AnnotationsController::Build()
{
    /* Main Layout */
    m_Layout = new QVBoxLayout(this);

    /* Button Group */
    m_AnnotationGroup = new QButtonGroup(this);
    m_AnnotationGroup->setExclusive(true);

    /* Slider */
    m_SizeSlider = new FramelessSlider(this);

    /* Buttons */
    m_PointerButton = new QPushButton;
    m_PointerButton->setIcon(QIcon(":resources/icons/icon_pointer.svg"));
    m_PointerButton->setCheckable(true);
    m_PointerButton->setFlat(true);
    m_PointerButton->setToolTip("<b>Pointer</b>");

    m_BrushButton = new QPushButton;
    m_BrushButton->setIcon(QIcon(":resources/icons/icon_brush.svg"));
    m_BrushButton->setCheckable(true);
    m_BrushButton->setFlat(true);
    m_BrushButton->setToolTip("<b>Brush Tool</b><br>Annotate free hand using the Brush tool.");

    m_TextButton = new QPushButton;
    m_TextButton->setIcon(QIcon(":resources/icons/icon_text.svg"));
    m_TextButton->setCheckable(true);
    m_TextButton->setFlat(true);
    m_TextButton->setToolTip("<b>Text Tool</b><br>Annotate by typing where clicked.");

    m_EraserButton = new QPushButton;
    m_EraserButton->setIcon(QIcon(":resources/icons/icon_eraser.svg"));
    m_EraserButton->setCheckable(true);
    m_EraserButton->setFlat(true);
    m_EraserButton->setToolTip("<b>Eraser Tool</b><br>Erase Text Characters or Annotation Stroke by dragging over.");

    m_ClearButton = new QPushButton;
    m_ClearButton->setIcon(QIcon(":resources/icons/icon_delete.svg"));
    m_ClearButton->setFlat(true);
    m_ClearButton->setToolTip("<b>Delete Annotation</b><br>Deletes the annotation from the current frame.");

    m_ColorButton = new ColorSelectionButton({255, 255, 255}, this);
    m_ColorButton->setToolTip("<b>Color Selector</b><br>Selects Color for Annotation.");

    m_SizeAdjuster = new QPushButton;
    m_SizeAdjuster->setIcon(QIcon(":resources/icons/icon_adjust.svg"));
    m_SizeAdjuster->setFlat(true);
    m_SizeAdjuster->setToolTip("<b>Adjust Size</b><br>Adjust size of the Brush/Eraser.");

    /* Add to the button group */
    m_AnnotationGroup->addButton(m_PointerButton, 0);
    m_AnnotationGroup->addButton(m_BrushButton, 1);
    m_AnnotationGroup->addButton(m_TextButton, 2);
    m_AnnotationGroup->addButton(m_EraserButton, 3);

    /* Add to the Main Layout */
    m_Layout->addWidget(m_PointerButton);
    m_Layout->addWidget(m_BrushButton);
    m_Layout->addWidget(m_TextButton);
    m_Layout->addWidget(m_EraserButton);
    m_Layout->addWidget(m_ClearButton);
    m_Layout->addWidget(m_ColorButton);
    m_Layout->addWidget(m_SizeAdjuster);

    /* Spacer */
    m_Layout->addStretch(1);
}

void AnnotationsController::Connect()
{
    /* Connect Outward Signals */
    connect(m_ClearButton, &QPushButton::clicked, this, &AnnotationsController::cleared);
    connect(m_ColorButton, &ColorSelectionButton::colorChanged, this, &AnnotationsController::colorChanged);
    connect(m_SizeAdjuster, &QPushButton::clicked, this, &AnnotationsController::OpenSizeSelector);
    connect(m_SizeSlider, &FramelessSlider::valueChanged, this, &AnnotationsController::brushSizeChanged);

    /* Control Changed */
    #if _QT6_COMPAT
    connect(m_AnnotationGroup, &QButtonGroup::idPressed, this, &AnnotationsController::controlChanged);
    #else
    connect(m_AnnotationGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonPressed), this, &AnnotationsController::controlChanged);
    #endif // _QT6
}

void AnnotationsController::Setup()
{
    /* Size Slider */
    m_SizeSlider->SetMinimum(2);
    m_SizeSlider->SetMaximum(14);
    m_SizeSlider->SetValue(2);

    /* Default Selection from the Button Group */
    m_PointerButton->setChecked(true);
}

void AnnotationsController::OpenSizeSelector()
{
    /* Show the Popup with the Slider */
    m_SizeSlider->show();
    /**
     * Move next to the Size button such that its on the left of the button 
     * (cuz the annotation controls are right most)
     * and at the center in terms of height
     */
    m_SizeSlider->move(m_SizeAdjuster->mapToGlobal(
        QPoint(-m_SizeSlider->width(), (m_SizeAdjuster->height() - m_SizeSlider->height()) / 2))
    );
}

VOID_NAMESPACE_CLOSE