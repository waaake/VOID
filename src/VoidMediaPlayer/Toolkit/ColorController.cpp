// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "ColorController.h"
#include "VoidCore/Processors/ColorProcessor.h"
#include "VoidIconForge/IconForge.h"
#include "VoidCore/Logging.h"
#include "VoidColor/ColorEngine.h"

VOID_NAMESPACE_OPEN

ColorController::ColorController(QWidget* parent)
    : ControlCombo(parent)
{
    Setup();
    connect(this, &QComboBox::currentTextChanged, this, [this](const QString& value) { emit colorDisplayChanged(value.toStdString());});
}

void ColorController::Setup()
{
    ColorProcessor& proc = ColorProcessor::Instance();
    for (std::string& display : proc.Displays())
        addItem(display.c_str());

    setCurrentText(proc.DefaultDisplay().c_str());
}

ColorController2::ColorController2(QWidget* parent)
    : QWidget(parent)
{
    Build();
}

ColorController2::~ColorController2()
{
    m_Layout->deleteLater();
    delete m_Layout;
    m_Layout = nullptr;
}

void ColorController2::Setup(ColorEngine* engine)
{
    m_Engine = engine;
    Reset();

    connect(m_Engine, &ColorEngine::displayChanged, this, &ColorController2::Reset);
    connect(m_ViewController, &QComboBox::currentTextChanged, engine, &ColorEngine::SetView);
}

void ColorController2::Build()
{
    m_Layout = new QHBoxLayout(this);
    m_ViewController = new ControlCombo(this);

    m_SettingsButton = new QToolButton(this);
    m_SettingsButton->setAutoRaise(true);
    m_SettingsButton->setIcon(IconForge::GetIcon(IconType::icon_manufacturing, _DARK_COLOR(QPalette::Text, 100)));

    m_Layout->addWidget(m_ViewController);
    m_Layout->addWidget(m_SettingsButton);
    m_Layout->setContentsMargins(0, 0, 0, 0);
}

void ColorController2::Reset()
{
    bool b = m_ViewController->blockSignals(true);

    m_ViewController->clear();
    m_ViewController->addItems(m_Engine->Views());
    m_ViewController->setCurrentText(m_Engine->ActiveView());

    m_ViewController->blockSignals(b);

    VOID_LOG_INFO("Reset Color Views");
}

VOID_NAMESPACE_CLOSE
