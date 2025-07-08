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

    /* This controller has a fixed width */
    setFixedWidth(40);
}

AnnotationsController::~AnnotationsController()
{
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

    m_SizeSlider->SetMinimum(1);
    m_SizeSlider->SetMaximum(10);
    m_SizeSlider->SetValue(2);

    /* Buttons */
    m_BrushButton = new QPushButton;
    m_BrushButton->setIcon(QIcon(":resources/icons/icon_brush.svg"));
    m_BrushButton->setCheckable(true);
    m_BrushButton->setFlat(true);

    m_TextButton = new QPushButton;
    m_TextButton->setIcon(QIcon(":resources/icons/icon_text.svg"));
    m_TextButton->setCheckable(true);
    m_TextButton->setFlat(true);

    m_EraserButton = new QPushButton;
    m_EraserButton->setIcon(QIcon(":resources/icons/icon_eraser.svg"));
    m_EraserButton->setCheckable(true);
    m_EraserButton->setFlat(true);

    m_ClearButton = new QPushButton;
    m_ClearButton->setIcon(QIcon(":resources/icons/icon_delete.svg"));
    m_ClearButton->setFlat(true);

    m_ColorButton = new ColorSelectionButton({230, 230, 23}, this);

    m_SizeAdjuster = new QPushButton;
    m_SizeAdjuster->setIcon(QIcon(":resources/icons/icon_adjust.svg"));
    m_SizeAdjuster->setFlat(true);

    /* Add to the button group */
    m_AnnotationGroup->addButton(m_BrushButton);
    m_AnnotationGroup->addButton(m_TextButton);
    m_AnnotationGroup->addButton(m_EraserButton);

    /* Add to the Main Layout */
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