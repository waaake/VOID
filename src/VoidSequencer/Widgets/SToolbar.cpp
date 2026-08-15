// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "SToolbar.h"
#include "VoidIconForge/IconForge.h"
#include "VoidQExtensions/Tooltip.h"

VOID_NAMESPACE_OPEN

SToolbar::SToolbar(QWidget* parent)
    : QFrame(parent)
{
    Build();
    Setup();
    Connect();

    setFixedWidth(Sequencer::ToolbarWidth);
}

void SToolbar::Build()
{
    m_ActionGroup.setExclusive(true);
    m_Layout = new QVBoxLayout(this);

    m_ResetButton = new QToolButton(this);
    m_ResetButton->setIcon(IconForge::GetIcon(IconType::icon_sync_alt, _DARK_COLOR(QPalette::Text, 100), 28));
    m_ResetButton->setFixedSize({32, 32});
    m_ResetButton->setAutoRaise(true);

    m_PointerButton = new HighlightToggleButton(this);
    m_PointerButton->setIcon(IconForge::GetIcon(IconType::icon_arrow_selector, _DARK_COLOR(QPalette::Text, 100), 28));
    m_PointerButton->setFixedSize({32, 32});
    m_PointerButton->setToolTip(ToolTipString("Pointer Context", "Resets the current action on the Sequencer Timeline.").c_str());

    m_SlipClipButton = new HighlightToggleButton(this);
    m_SlipClipButton->setIcon(IconForge::GetIcon(IconType::icon_arrow_range, _DARK_COLOR(QPalette::Text, 100), 28));
    m_SlipClipButton->setFixedSize({32, 32});
    m_SlipClipButton->setToolTip(ToolTipString("Slip Clip", "Slip media content over the track item range.").c_str());

    m_TrimButton = new HighlightToggleButton(this);
    m_TrimButton->setIcon(IconForge::GetIcon(IconType::icon_format_overflow, _DARK_COLOR(QPalette::Text, 100), 28));
    m_TrimButton->setFixedSize({32, 32});
    m_TrimButton->setToolTip(ToolTipString("Trim", "Trim item ranges (timeline in/out) through head and tail handle markers on track items.").c_str());

    m_RazorButton = new HighlightToggleButton(this);
    m_RazorButton->setIcon(IconForge::GetIcon(IconType::icon_bolt, _DARK_COLOR(QPalette::Text, 100), 28));
    m_RazorButton->setFixedSize({32, 32});
    m_RazorButton->setToolTip(ToolTipString("Razor", "Cut track item on a given track at the frame.").c_str());

    m_RazorAllButton = new HighlightToggleButton(this);
    m_RazorAllButton->setIcon(IconForge::GetIcon(IconType::icon_carpenter, _DARK_COLOR(QPalette::Text, 100), 28));
    m_RazorAllButton->setFixedSize({32, 32});
    m_RazorAllButton->setToolTip(ToolTipString("Razor All", "Cut track items on all the tracks in the sequence at the frame.").c_str());

    m_MergeButton = new HighlightToggleButton(this);
    m_MergeButton->setIcon(IconForge::GetIcon(IconType::icon_cell_merge, _DARK_COLOR(QPalette::Text, 100), 28));
    m_MergeButton->setFixedSize({32, 32});
    m_MergeButton->setToolTip(ToolTipString("Merge Cut", "Merge cuts created by the Razor tool.").c_str());

    m_ActionGroup.addButton(m_PointerButton, static_cast<int>(SequencerAction::NONE));
    m_ActionGroup.addButton(m_SlipClipButton, static_cast<int>(SequencerAction::SLIP_CLIP));
    m_ActionGroup.addButton(m_TrimButton, static_cast<int>(SequencerAction::TRIM));
    m_ActionGroup.addButton(m_RazorButton, static_cast<int>(SequencerAction::RAZOR));
    m_ActionGroup.addButton(m_RazorAllButton, static_cast<int>(SequencerAction::RAZOR_ALL));
    m_ActionGroup.addButton(m_MergeButton, static_cast<int>(SequencerAction::MERGE));

    m_Layout->addWidget(m_ResetButton);
    m_Layout->addWidget(m_PointerButton);
    m_Layout->addWidget(m_SlipClipButton);
    m_Layout->addWidget(m_TrimButton);
    m_Layout->addWidget(m_RazorButton);
    m_Layout->addWidget(m_RazorAllButton);
    m_Layout->addWidget(m_MergeButton);
    m_Layout->setAlignment(Qt::AlignTop);

    QMargins margins = m_Layout->contentsMargins();
    margins.setLeft(4);
    margins.setRight(4);
    m_Layout->setContentsMargins(margins);
}

void SToolbar::Connect()
{
    connect(m_ResetButton, &QToolButton::clicked, this, &SToolbar::reset);
    #if _QT6_COMPAT
    connect(&m_ActionGroup, &QButtonGroup::idPressed, this, &SToolbar::ActionSwitched);
    #else
    connect(&m_ActionGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonPressed), this, &SToolbar::ActionSwitched);
    #endif // _QT6_COMPAT
}

void SToolbar::Setup()
{
    m_PointerButton->setChecked(true);
}

void SToolbar::ActionSwitched(int action)
{
    emit actionSwitched(static_cast<SequencerAction>(action));
}

VOID_NAMESPACE_CLOSE
