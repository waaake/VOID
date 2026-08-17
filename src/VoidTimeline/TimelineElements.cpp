// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QAction>
#include <QMenu>
#include <QPalette>
#include <QValidator>
#include <QLineEdit>

/* Internal */
#include "TimelineElements.h"
#include "VoidCore/Logging.h"
#include "VoidCore/Timekeeper.h"
#include "VoidIconForge/IconForge.h"

VOID_NAMESPACE_OPEN

/* Loop Type Button {{{ */

LoopTypeButton::LoopTypeButton(QWidget* parent)
	: QPushButton(parent)
	, m_LoopType(LoopType::LoopInfinitely)
{
	/* Update Loop States to be used on the button */
	m_LoopState[LoopType::LoopInfinitely] = {"Loop Infinitely", IconType::icon_repeat};
	m_LoopState[LoopType::LoopOneInfinitely] = {"Loop One", IconType::icon_repeat_one};
	m_LoopState[LoopType::PingPong] = {"Bounce", IconType::icon_sync_alt};
	m_LoopState[LoopType::PlayOnce] = {"Play Once", IconType::icon_trending_flat};

	/* Update to set the state on the button */
	Build();

	/* No Visible borders */
	setFlat(true);
}

LoopTypeButton::~LoopTypeButton()
{
	m_Menu->deleteLater();
}

void LoopTypeButton::SetLoopType(const LoopType& looptype)
{
	/* Update Loop state */
	m_LoopType = looptype;

	/* Reset Icon */
	Update();

	/* Emit that the loop type has been changed */
	emit loopTypeChanged(looptype);
}

void LoopTypeButton::Build()
{
	/* Update the state on the button */
	Update();

	/* Add menu for Loop Type Selection */
	m_Menu = new QMenu(this);

	for (const std::pair<LoopType, LoopState>& entry: m_LoopState)
	{
		QAction* action = new QAction(IconForge::GetIcon(entry.second.icon, _DARK_COLOR(QPalette::Text, 140)), entry.second.text.c_str(), m_Menu);
		connect(action, &QAction::triggered, this, [this, entry]() { SetLoopType(entry.first); });
		m_Menu->addAction(action);
	}

	/* Set the Menu on the button */
	setMenu(m_Menu);
}

void LoopTypeButton::Update()
{
	setIcon(IconForge::GetIcon(m_LoopState.at(m_LoopType).icon, _DARK_COLOR(QPalette::Text, 140)));
}

/* }}} */

/* TimeDisplay {{{ */

TimeDisplay::TimeDisplay(QWidget* parent)
    : QLabel(parent)
{
    Setup();
}

void TimeDisplay::Setup()
{
    /* How the UI Apears */
	setEnabled(false);
	setAlignment(Qt::AlignCenter);

    /* Fixed Size? */
	setFixedWidth(80);

    /* Setup the Colors */
    QPalette p = palette();
    p.setColor(QPalette::WindowText, p.color(QPalette::Highlight));
    setPalette(p);
    
    /* Setup Font */
    QFont f = font();
    f.setBold(true);
    setFont(f);
}

/* }}} */

/* Framerate Box {{{ */

FramerateBox::FramerateBox(QWidget* parent)
	: QComboBox(parent)
{
	/* Validator */
	m_DoubleValidator = new QDoubleValidator(this);
	m_DoubleValidator->setBottom(0.0);
	m_DoubleValidator->setTop(2000.0);

	/* Setup the Box */
	Setup();

	connect(this, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [this](int) { RateChanged(currentText()); });
	connect(this->lineEdit(), &QLineEdit::returnPressed, this, [this]() { RateChanged(currentText()); });
	connect(this->lineEdit(), &QLineEdit::editingFinished, this, [this]() { RateChanged(currentText()); });
}

void FramerateBox::Setup()
{
	// Setup values and defaults
	const QStringList values = {
		"8",
		"10",
		"12",
		"12.50",
		"15",
		"23.98",
		"24",
		"25",
		"29.97",
		"30",
		"48",
		"50",
		"59.94",
		"60"
	};

	addItems(values);
	setEditable(true);

	setValidator(m_DoubleValidator);
	setFocusPolicy(Qt::ClickFocus);

	QPalette p = palette();
    // p.setColor(QPalette::Text, p.color(QPalette::Highlight));
	p.setColor(QPalette::Base, p.color(QPalette::Dark));
    setPalette(p);
}

void FramerateBox::RateChanged(const QString& text)
{
	Timekeeper::Instance().SetFramerate(text.toDouble());
	emit framerateChanged(text.toDouble());

	VOID_LOG_INFO("Playback Framerate changed: {0}", text.toStdString());

	/* Unset Focus once the text is updated */
	clearFocus();
}

/* }}} */

/// TimeEdit

TimeEdit::TimeEdit(QWidget* parent)
	: QLineEdit(parent)
{
	setValidator(new QIntValidator(this));
	setAlignment(Qt::AlignCenter);

    QPalette p = palette();
    p.setColor(QPalette::Text, p.color(QPalette::Highlight));
	p.setColor(QPalette::Base, p.color(QPalette::Dark));
    setPalette(p);

    QFont f = font();
    f.setBold(true);
    setFont(f);
}

VOID_NAMESPACE_CLOSE
