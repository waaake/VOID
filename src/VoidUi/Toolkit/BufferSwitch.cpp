// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>

/* Internal */
#include "BufferSwitch.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

static const QColor INACTIVE_BUFFER_COLOR = QColor(90, 90, 90);

/* Buffer Page {{{ */

BufferPage::BufferPage(ViewerBuffer* viewBuffer, const PlayerViewBuffer& buffer, QWidget* parent)
    : QFrame(parent)
    , m_ViewerBuffer(viewBuffer)
    , m_Buffer(buffer)
{
    /* Default size */
    setFixedWidth(30);

    /* Connect to ViewerBuffer Update */
    connect(m_ViewerBuffer, &ViewerBuffer::updated, this, [this]() { update(); });
}

BufferPage::~BufferPage()
{
}

void BufferPage::paintEvent(QPaintEvent* event)
{
    /* Setup Painter */
    QPainter painter(this);

    QColor color = m_ViewerBuffer->Active() ? m_ViewerBuffer->Color() : palette().color(QPalette::Disabled, QPalette::Text);

    /* Draw Border */
    painter.setPen(QPen(color, 2));
    painter.drawRect(rect());

    /* Draw Indication rect */
    painter.fillRect(QRect(2, 2, 4, height() - 4), color);
    /* And the name */
    painter.drawText(rect(), Qt::AlignCenter, m_ViewerBuffer->Name().c_str());
}

void BufferPage::mousePressEvent(QMouseEvent* event)
{
    /* Emit that this was clicked along with it's Buffer state */
    emit selected(m_Buffer);

    /* Base Mouse Press */
    QFrame::mousePressEvent(event);
}

/* }}} */

/* Comparison Mode Selector {{{ */

ComparisonModeSelector::ComparisonModeSelector(QWidget* parent)
    : SplitSectionSelector(parent)
{
    /* Setup UI values */
    Setup();
}

void ComparisonModeSelector::Setup()
{
    /* Add the Primary Menu Items */
    QStringList comparisonModes;
    QStringList blendModes;

    /* Add the Renderer Comparison Modes */
    for (auto it: Renderer::ComparisonModesMap)
        comparisonModes << it.second.c_str();

    /* And the Renderer Blend Modes */
    for (auto it: Renderer::BlendModesMap)
        blendModes << it.second.c_str();

    AddPrimaryItems(comparisonModes);
    /* Add a Separator After the Primary Items */
    AddSeparator();

    /* Radio Items */
    AddRadioItems(blendModes);

    /* Set the Default Radio Item check state */
    m_RadioActions.front()->setChecked(true);
}

void ComparisonModeSelector::SetCompareMode(const Renderer::ComparisonMode& mode)
{
    /* Update the Text on the Frame */
    setText(Renderer::ComparisonModesMap.at(mode).c_str());

    /* Emit the index of the Mode so that other components can receive them */
    emit primaryIndexChanged(static_cast<int>(mode));
}

/* }}} */

/* Buffer Switch {{{ */

BufferSwitch::BufferSwitch(ViewerBuffer* A, ViewerBuffer* B, QWidget* parent)
    : QWidget(parent)
    , m_ViewerBufferA(A)
    , m_ViewerBufferB(B)
{
    /* Build the UI */
    Build();

    /* Connect Signals */
    Connect();
}

BufferSwitch::~BufferSwitch()
{
}

void BufferSwitch::Build()
{
    /* Base Layout */
    m_Layout = new QHBoxLayout(this);
    /* Spacing */
    m_Layout->setContentsMargins(0, 0, 0, 0);
    m_Layout->setSpacing(0);

    m_BufferA = new BufferPage(m_ViewerBufferA, PlayerViewBuffer::A);
    m_BufferB = new BufferPage(m_ViewerBufferB, PlayerViewBuffer::B);

    m_ComparisonModes = new ComparisonModeSelector(this);

    /* Add to the Control Layout */
    m_Layout->addWidget(m_BufferA);
    m_Layout->addWidget(m_ComparisonModes);
    m_Layout->addWidget(m_BufferB);
}

void BufferSwitch::Connect()
{
    /* Connect the buffer pages to switch on click */
    connect(m_BufferA, &BufferPage::selected, this, &BufferSwitch::switched);
    connect(m_BufferB, &BufferPage::selected, this, &BufferSwitch::switched);

    /* Connect the Comparison Mode changes */
    connect(m_ComparisonModes, &SplitSectionSelector::primaryIndexChanged, this, &BufferSwitch::compareModeChanged);
    connect(m_ComparisonModes, &SplitSectionSelector::radioIndexChanged, this, &BufferSwitch::blendModeChanged);
}

/* }}} */

VOID_NAMESPACE_CLOSE
