// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QLabel>
#include <QPainter>

/* Internal */
#include "Editor.h"
#include "VoidIconForge/IconForge.h"
#include "VoidQExtensions/Frame.h"
#include "VoidQExtensions/Tooltip.h"

VOID_NAMESPACE_OPEN

inline constexpr int BUTTON_SIZE = 16;

BaseEditor::BaseEditor(QWidget* parent)
    : QWidget(parent)
    , m_Row(-1)
    , m_Column(-1)
{
    Build();
    Setup();
}

void BaseEditor::Close()
{
    hide();
    setParent(nullptr);
    deleteLater();
}

void BaseEditor::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    const QRect r = rect();
    painter.save();

    painter.setPen(QPen(QBrush(Qt::black), 1));
    painter.setBrush(QBrush(palette().color(QPalette::Window)));
    painter.drawRect(r.adjusted(0, 0, -1, -1));

    painter.setBrush(QBrush(palette().color(QPalette::Dark)));
    painter.drawRect(0, 0, r.width() - 1, 30);
    painter.restore();

    // Node color on the bottom as well
    painter.fillRect(QRect(0, r.height() - 2, r.width() - 1, 2), m_ColorButton->Color());

    QWidget::paintEvent(event); 
}

void BaseEditor::SetName(const QString& name, bool readonly)
{
    m_NameEdit->setText(name);
    m_NameEdit->setReadOnly(readonly);
}

void BaseEditor::SetColor(const QColor& color)
{
    m_ColorButton->SetColor(color);
}

void BaseEditor::Enable(bool enabled)
{
    // m_EnableButton->toggle
}

void BaseEditor::AddControl(const EditorControl& editor, bool newline)
{
    if (newline)
    {
        ++m_Row;
        m_Column = -1;
    }

    editor.widget->setObjectName(editor.name);
    editor.widget->setToolTip(ToolTipString(editor.name, editor.description));
    m_Grid->addWidget(new QLabel(editor.label, this), m_Row, ++m_Column, Qt::AlignRight);
    m_Grid->addWidget(editor.widget, m_Row, ++m_Column, 1, -1); // Occupy full width
}

void BaseEditor::AddControl(const EditorControl& editor, int rspan, int cspan, bool newline)
{
    if (newline)
    {
        ++m_Row;
        m_Column = -1;
    }

    editor.widget->setObjectName(editor.name);
    editor.widget->setToolTip(ToolTipString(editor.name, editor.description));
    m_Grid->addWidget(new QLabel(editor.label, this), m_Row, ++m_Column, Qt::AlignRight);
    m_Grid->addWidget(editor.widget, m_Row, ++m_Column, rspan, cspan);
}

void BaseEditor::AddWidget(const EditorControl& editor, bool newline)
{
    if (newline)
    {
        ++m_Row;
        m_Column = -1;
    }

    editor.widget->setObjectName(editor.name);
    editor.widget->setToolTip(ToolTipString(editor.name, editor.description));
    m_Grid->addWidget(editor.widget, m_Row, ++m_Column);
}

void BaseEditor::AddWidget(const EditorControl& editor, int rspan, int cspan, bool newline)
{
    if (newline)
    {
        ++m_Row;
        m_Column = -1;
    }

    editor.widget->setObjectName(editor.name);
    editor.widget->setToolTip(ToolTipString(editor.name, editor.description));
    m_Grid->addWidget(editor.widget, m_Row, ++m_Column, rspan, cspan);
}

void BaseEditor::BeginSection(const QString& label)
{
    QHBoxLayout* l = new QHBoxLayout();

    QLabel* name = new QLabel(label, this);
    QFont f(name->font());
    f.setBold(true);
    name->setFont(f);
    name->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    l->addWidget(name);
    l->addWidget(new HLine(this));

    m_Grid->addLayout(l, ++m_Row, 0, 1, -1);
}

void BaseEditor::AddSeparator()
{
    m_Grid->addWidget(new HLine(this), ++m_Row, 0, 1, -1);
}

QWidget* BaseEditor::Control(const QString& name)
{
    return findChild<QWidget*>(name);
}

void BaseEditor::Build()
{
    m_Layout = new QVBoxLayout(this);
    m_Grid = new QGridLayout;

    QHBoxLayout* title = new QHBoxLayout;
    title->setContentsMargins(2, 0, 6, 8);

    m_NameEdit = new QLineEdit;

    m_ColorButton = new ColorSelectionButton(this);
    m_EnableButton = new QToolButton(this);
    m_RefreshButton = new QToolButton(this);
    m_CloseButton = new CloseButton(this);

    title->addWidget(m_NameEdit);
    title->addWidget(m_ColorButton);
    title->addWidget(m_EnableButton);
    title->addWidget(m_RefreshButton);
    title->addWidget(m_CloseButton);

    m_Layout->addLayout(title);
    m_Layout->addLayout(m_Grid);
}

void BaseEditor::Setup()
{
    m_Layout->setAlignment(Qt::AlignTop);

    int bottom;
    m_Layout->getContentsMargins(nullptr, nullptr, nullptr, &bottom);
    m_Layout->setContentsMargins(1, 4, 1, bottom);

    m_Grid->setContentsMargins(8, 0, 8, 8);

    QIcon icon;
    icon.addPixmap(IconForge::GetPixmap(IconType::icon_visible, _DARK_COLOR(QPalette::Text, 100)), QIcon::Normal, QIcon::On);
    icon.addPixmap(IconForge::GetPixmap(IconType::icon_visible_off, _DARK_COLOR(QPalette::Text, 100)), QIcon::Normal, QIcon::Off);

    m_EnableButton->setIcon(icon);
    m_EnableButton->setCheckable(true);
    m_EnableButton->setAutoRaise(true);

    m_RefreshButton->setIcon(IconForge::GetIcon(IconType::icon_refresh, _DARK_COLOR(QPalette::Text, 100)));
    m_RefreshButton->setAutoRaise(true);

    m_EnableButton->setFixedSize(BUTTON_SIZE, BUTTON_SIZE);
    m_RefreshButton->setFixedSize(BUTTON_SIZE, BUTTON_SIZE);
    m_CloseButton->setFixedSize(BUTTON_SIZE, BUTTON_SIZE);
    m_ColorButton->setFixedSize(BUTTON_SIZE, BUTTON_SIZE);
    
    // Signals
    connect(m_NameEdit, &QLineEdit::editingFinished, this, [this]() -> void { emit nameChangeRequested(m_NameEdit->text()); });
    connect(m_EnableButton, &QToolButton::toggled, this, &BaseEditor::stateChangeRequested);
    connect(m_RefreshButton, &QToolButton::clicked, this, [this]() -> void 
    {
        bool blocked = blockSignals(true);
        // Ensure that refresh doesn't trigger whole range of updates to the actual node...
        Refresh();
        blockSignals(blocked);
    });
    connect(m_CloseButton, &CloseButton::clicked, this, &BaseEditor::Close);
    connect(m_ColorButton, &ColorSelectionButton::colorChanged, this, [this](const QColor& color) -> void
    {
        update();
        emit colorChangeRequested(color);
    });
}

VOID_NAMESPACE_CLOSE
