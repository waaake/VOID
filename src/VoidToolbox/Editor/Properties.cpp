// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>
#include <QScrollArea>
#include <QValidator>

/* Internal */
#include "Properties.h"
#include "Color.h"
#include "Sequence.h"
#include "VoidIconForge/IconForge.h"
#include "VoidQExtensions/Frame.h"

VOID_NAMESPACE_OPEN

PropertiesPanel::PropertiesPanel(QWidget* parent)
    : QWidget(parent)
{
    Build();
    Setup();
    Connect();
}

PropertiesPanel::~PropertiesPanel()
{
    m_ScrollLayout->deleteLater();
    delete m_ScrollLayout;
    m_ScrollLayout = nullptr;

    m_Layout->deleteLater();
    delete m_Layout;
    m_Layout = nullptr;
}

void PropertiesPanel::Show(QWidget* panel)
{
    m_ScrollLayout->addWidget(panel);
}

void PropertiesPanel::EditEffect(Effect* effect)
{
    // Remove any existing Editors for the same Effect before adding this again
    for (auto* w : findChildren<EffectEditor*>())
    {
        if (w->HasSameEffect(effect))
            w->Close();
    }

    // Ensure we have only panels till the limit
    // Going one lesser here as we are also adding another widget
    ClearAdditionalPanels(m_PanelCounter->CurrentValue() - 1);
    m_ScrollLayout->insertWidget(0, new EffectEditor(effect, this));
}

void PropertiesPanel::EditMedia(const SharedMediaClip& media)
{
    for (auto* w : findChildren<MediaEditor*>())
    {
        if (w->HasSameMedia(media))
            w->Close();
    }

    // Ensure we have only panels till the limit
    // Going one lesser here as we are also adding another widget
    ClearAdditionalPanels(m_PanelCounter->CurrentValue() - 1);

    auto* editor = new MediaEditor(media, this);
    connect(editor, &MediaEditor::mediaUpdated, this, &PropertiesPanel::mediaReceivedUpdate);
    m_ScrollLayout->insertWidget(0, editor);
}

void PropertiesPanel::EditSequence(const SharedPlaybackSequence& sequence)
{
    for (auto* w : findChildren<SequenceEditor*>())
    {
        if (w->HasSameSequence(sequence))
            w->Close();
    }

    // Ensure we have only panels till the limit
    // Going one lesser here as we are also adding another widget
    ClearAdditionalPanels(m_PanelCounter->CurrentValue() - 1);

    auto* editor = new SequenceEditor(sequence, this);
    m_ScrollLayout->insertWidget(0, editor);
}

void PropertiesPanel::Inspect(ColorEngine* engine)
{
    for (auto* w : findChildren<ColorConfigEditor*>())
    {
        if (w->HasSameEngine(engine))
            w->Close();
    }

    // Ensure we have only panels till the limit
    // Going one lesser here as we are also adding another widget
    ClearAdditionalPanels(m_PanelCounter->CurrentValue() - 1);

    auto* editor = new ColorConfigEditor(engine, this);
    m_ScrollLayout->insertWidget(0, editor);
}

void PropertiesPanel::Clear()
{
    for (auto w : findChildren<BaseEditor*>())
        w->Close();
}

void PropertiesPanel::Remove(const Effect* effect)
{
    if (auto w = findChild<EffectEditor*>())
    {
        if (w->HasSameEffect(effect))
            w->Close();
    }
}

void PropertiesPanel::ClearAdditionalPanels(int limit)
{
    QList<BaseEditor*> panels = findChildren<BaseEditor*>();
    if (panels.size() >= limit)
    {
        for (int i = 0; i < panels.size() - limit; ++i)
            panels.at(i)->Close();
    }
}

void PropertiesPanel::Build()
{
    m_Layout = new QVBoxLayout(this);

    QHBoxLayout* optionsLayout = new QHBoxLayout;
    optionsLayout->setContentsMargins(8, 0, 8, 0);
    m_ClearButton = new QPushButton;
    m_PanelCounter = new IntBoundLineEdit(1, 20, 10, this);

    optionsLayout->addWidget(m_PanelCounter);
    optionsLayout->addStretch(1);
    optionsLayout->addWidget(m_ClearButton);

    BaseWidget* scrollWidget = new BaseWidget;
    scrollWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

    m_ScrollLayout = new QVBoxLayout(scrollWidget);
    m_ScrollLayout->setContentsMargins(4, 4, 4, 0);

    QScrollArea* scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(scrollWidget);

    m_Layout->addLayout(optionsLayout);
    m_Layout->addWidget(scrollArea);
    m_Layout->setContentsMargins(1, 8, 1, 1);
}

void PropertiesPanel::Setup()
{
    m_PanelCounter->setFixedWidth(25);

    m_ClearButton->setFixedSize(25, 25);
    m_ClearButton->setIcon(IconForge::GetIcon(IconType::icon_clear_all, _DARK_COLOR(QPalette::Text, 100)));
}

void PropertiesPanel::Connect()
{
    connect(m_ClearButton, &QPushButton::clicked, this, &PropertiesPanel::Clear);
    connect(m_PanelCounter, &IntBoundLineEdit::currentValueChanged, this, &PropertiesPanel::ClearAdditionalPanels);
}

VOID_NAMESPACE_CLOSE
