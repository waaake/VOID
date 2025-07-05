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

    QColor color = m_ViewerBuffer->Active() ? m_ViewerBuffer->Color() : INACTIVE_BUFFER_COLOR;

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

    m_ComparisonModes = new QComboBox;
    // m_ComparisonModes->addItems({"Under", "Over", "Add", "Subtract", "Mix"});
    m_ComparisonModes->addItems({"Off", "Wipe", "Stack"});

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
    connect(m_ComparisonModes, static_cast<void (QComboBox::*) (int)>(&QComboBox::currentIndexChanged), this, &BufferSwitch::compareModeChanged);
}

/* }}} */

VOID_NAMESPACE_CLOSE
