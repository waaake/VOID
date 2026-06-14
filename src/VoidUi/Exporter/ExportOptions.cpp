// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QLabel>

/* Internal */
#include "ExportOptions.h"
#include "VoidCore/ColorProcessor.h"
#include "VoidUi/Engine/IconForge.h"
#include "VoidUi/Tools/Delegates/LogDelegate.h"

VOID_NAMESPACE_OPEN

/// Movie Options

MovieOptions::MovieOptions(QWidget* parent)
    : QWidget(parent)
{
    Build();
    Setup();
    Connect();
}

MovieOptions::~MovieOptions()
{
    m_Layout->deleteLater();
    delete m_Layout;
    m_Layout = nullptr;
}

void MovieOptions::SetRate(double rate)
{
    m_RateEdit->setText(QString::number(rate, 'f', 1));
}

void MovieOptions::Build()
{
    m_RateValidator = new QDoubleValidator(this);

    m_Layout = new QVBoxLayout(this);

    m_CodecCombo = new QComboBox;
    m_RateEdit = new QLineEdit;
    // m_RespeedCheck = new QCheckBox;

    QGridLayout* optionsLayout = new QGridLayout;
    optionsLayout->addWidget(new QLabel("Codec:", this), 0, 0, 1, 1);
    optionsLayout->addWidget(m_CodecCombo, 0, 1, 1, 3);
    optionsLayout->addWidget(new QLabel("Framerate:", this), 1, 0, 1, 1);
    optionsLayout->addWidget(m_RateEdit, 1, 1, 1, 3);
    // optionsLayout->addWidget(new QLabel("Respeed:", this), 2, 0, 1, 1);
    // optionsLayout->addWidget(m_RespeedCheck, 2, 1, 1, 2);

    m_Layout->addLayout(optionsLayout);
    m_Layout->setContentsMargins(0, 0, 0, 0);
}

void MovieOptions::Setup()
{
    m_CodecCombo->addItems({
        "H.264",
        "DNxHD",
        "MJpeg",
        "Mpeg4",
        "ProRes"
    });

    m_RateEdit->setValidator(m_RateValidator);
}

void MovieOptions::Connect()
{

}

/// Export Options

ExportOptions::ExportOptions(QWidget* parent)
    : QDialog(parent)
{
    Build();
    Setup();
    Connect();
}

ExportOptions::~ExportOptions()
{
    m_Layout->deleteLater();
    delete m_Layout;
    m_Layout = nullptr;
}

void ExportOptions::SetRange(int start, int end)
{
    m_RangeValidator->setBottom(start);
    m_RangeValidator->setTop(end);

    m_StartEdit->setText(QString::number(start));
    m_EndEdit->setText(QString::number(end));
}

void ExportOptions::Build()
{
    m_LogModel = new TaskLogModel(this);
    m_RangeValidator = new QIntValidator(this);

    m_Layout = new QVBoxLayout(this);

    QHBoxLayout* inputLayout = new QHBoxLayout;

    m_OutputEdit = new QLineEdit;
    m_BrowseButton = new QPushButton;

    inputLayout->addWidget(m_OutputEdit);
    inputLayout->addWidget(m_BrowseButton);

    QGridLayout* optionsLayout = new QGridLayout;

    // QHBoxLayout* rangeLayout = new QHBoxLayout;

    m_StartEdit = new QLineEdit;
    m_EndEdit = new QLineEdit;

    m_OverrideRangeCheck = new QCheckBox("Override Media Range");

    m_ResolutionCombo = new QComboBox;
    m_OutColorspaceCombo = new QComboBox;
    m_OutProcessorCombo = new QComboBox;

    m_MovieGroup = new QGroupBox("Movie Options:");
    QHBoxLayout* mgroupLayout = new QHBoxLayout(m_MovieGroup);

    m_MovieOptions = new MovieOptions;
    mgroupLayout->addWidget(m_MovieOptions);

    m_Logger = new QListView;

    optionsLayout->addWidget(new QLabel("Render Range:", this), 0, 0, 1, 1);
    optionsLayout->addWidget(m_StartEdit, 0, 1, 1, 1);
    optionsLayout->addWidget(m_EndEdit, 0, 2, 1, 1);
    optionsLayout->addWidget(m_OverrideRangeCheck, 0, 3, 1, 2);    
    optionsLayout->addWidget(new QLabel("Render Resolution:"), 1, 0, 1, 1);
    optionsLayout->addWidget(m_ResolutionCombo, 1, 1, 1, 3);
    optionsLayout->addWidget(new QLabel("Out Colorspace:"), 2, 0, 1, 1);
    optionsLayout->addWidget(m_OutColorspaceCombo, 2, 1, 1, 3);
    optionsLayout->addWidget(new QLabel("Process as:"), 3, 0, 1, 1);
    optionsLayout->addWidget(m_OutProcessorCombo, 3, 1, 1, 3);
    optionsLayout->addWidget(m_MovieGroup, 4, 0, 2, 5);
    optionsLayout->addWidget(m_Logger, 6, 0, 4, 5);

    QHBoxLayout* buttonLayout = new QHBoxLayout;

    m_ExportButton = new QPushButton("Export");
    m_CancelButton = new QPushButton("Cancel");

    buttonLayout->addStretch(1);
    buttonLayout->addWidget(m_ExportButton);
    buttonLayout->addWidget(m_CancelButton);

    m_Layout->addLayout(inputLayout);
    m_Layout->addLayout(optionsLayout);

    m_Layout->addStretch(1);
    m_Layout->addLayout(buttonLayout);
}

void ExportOptions::Setup()
{
    m_BrowseButton->setIcon(style()->standardIcon(style()->SP_DirOpenIcon));
    m_ResolutionCombo->addItems({
        "Same as input sequence",
        "Half resolution",
        "Quarter resolution",
    });
    m_OutProcessorCombo->addItems({
        "Image Sequence",
        "Movie"
    });

    for (const auto& colorspace : ColorProcessor::Instance().Colorspaces())
    {
        m_OutColorspaceCombo->addItem(
            IconForge::GetIcon(IconType::icon_deployed_cube, _DARK_COLOR(QPalette::Text, 100)),
            colorspace.c_str()
        );
    }

    m_MovieGroup->setVisible(false);

    m_StartEdit->setEnabled(m_OverrideRangeCheck->isChecked());
    m_EndEdit->setEnabled(m_OverrideRangeCheck->isChecked());

    // Read-only for the user to only see, selection only happens through the browse option
    m_OutputEdit->setEnabled(false);

    m_StartEdit->setValidator(m_RangeValidator);
    m_EndEdit->setValidator(m_RangeValidator);

    m_Logger->setModel(m_LogModel);
    m_Logger->setItemDelegate(new ColoredLogDelegate(m_Logger));
}

void ExportOptions::Connect()
{
    connect(m_BrowseButton, &QPushButton::clicked, this, &ExportOptions::Browse);
    connect(m_OutProcessorCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [this](int index) -> void
    {
        m_MovieGroup->setVisible(index == 1);
        // Whenever the Process as mode changes, simplest is to clear the path selected by the user as opposed to
        // Guessing the extension and setting it as a default
        m_OutputEdit->clear();
    });
    connect(m_OverrideRangeCheck, &QCheckBox::toggled, this, [this](bool checked) -> void
    {
        m_StartEdit->setEnabled(checked);
        m_EndEdit->setEnabled(checked);
    });
    connect(m_ExportButton, &QPushButton::clicked, this, &ExportOptions::exported);
    connect(m_CancelButton, &QPushButton::clicked, this, &ExportOptions::close);
}

void ExportOptions::Browse()
{
    MediaExportBrowser browser;
    if (browser.Save())
    {
        m_Descriptor = browser.File();
        m_OutputEdit->setText(m_Descriptor.entry.Fullpath().c_str());

        bool blocked = m_OutProcessorCombo->blockSignals(true);

        m_OutProcessorCombo->setCurrentIndex(static_cast<int>(m_Descriptor.type));
        m_MovieGroup->setVisible(static_cast<int>(m_Descriptor.type) == 1);

        m_OutProcessorCombo->blockSignals(blocked);
    }
}

void ExportOptions::Log(const QString& text, const TaskLog::Level& level)
{
    m_LogModel->AddLog(text, level);
}

VOID_NAMESPACE_CLOSE
