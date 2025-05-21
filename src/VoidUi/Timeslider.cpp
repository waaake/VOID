/* Internal */
#include "Timeslider.h"

/* Qt */
#include <QLayout>
#include <QIcon>
#include <QStyle>
#include <QValidator>

static const int BUTTON_WIDTH = 30;

VOID_NAMESPACE_OPEN

Timeslider::Timeslider(QWidget* parent)
	: QWidget(parent)
{
	/* Build the layout of the widget */
	Build();

	/* Stylize the components */
	Stylize();

	/* Setup any values */
	Setup();

	/* Connect Signals */
	Connect();
}

Timeslider::~Timeslider()
{
}

void Timeslider::Build()
{
	m_ForwardsTimer = new QTimer;
	m_BackwardsTimer = new QTimer;

	/* Validator */
	m_DoubleValidator = new QDoubleValidator;
	m_DoubleValidator->setBottom(0.0);
	m_DoubleValidator->setTop(200.0);

	/* Base Layout */
	QVBoxLayout* layout = new QVBoxLayout(this);
	QHBoxLayout* optionsLayout = new QHBoxLayout;

	QStyle* s = style();

	/* Forwards */
	m_ForwardButton = new QPushButton();
	m_ForwardButton->setIcon(s->standardIcon(s->SP_MediaPlay));
	m_ForwardButton->setFixedWidth(BUTTON_WIDTH);

	m_NextFrameButton = new QPushButton();
	m_NextFrameButton->setIcon(s->standardIcon(s->SP_MediaSeekForward));
	m_NextFrameButton->setFixedWidth(BUTTON_WIDTH);

	m_EndFrameButton = new QPushButton();
	m_EndFrameButton->setIcon(s->standardIcon(s->SP_MediaSkipForward));
	m_EndFrameButton->setFixedWidth(BUTTON_WIDTH);

	/* Backwards */
	m_BackwardButton = new QPushButton();

	/* Reversed Play icon for backward button */
	QPixmap p = s->standardPixmap(s->SP_MediaPlay);
	QImage backImg(p.toImage().mirrored(true, false));
	m_BackwardButton->setIcon(QIcon(QPixmap::fromImage(backImg)));
	m_BackwardButton->setFixedWidth(BUTTON_WIDTH);

	m_PrevFrameButton = new QPushButton();
	m_PrevFrameButton->setIcon(s->standardIcon(s->SP_MediaSeekBackward));
	m_PrevFrameButton->setFixedWidth(BUTTON_WIDTH);

	m_StartFrameButton = new QPushButton();
	m_StartFrameButton->setIcon(s->standardIcon(s->SP_MediaSkipBackward));
	m_StartFrameButton->setFixedWidth(BUTTON_WIDTH);

	/* Stop and others */
	m_StopButton = new QPushButton();
	m_StopButton->setIcon(s->standardIcon(s->SP_MediaStop));
	m_StopButton->setFixedWidth(BUTTON_WIDTH);

	/* Timing */
	m_TimeDisplay = new QLineEdit;
	m_TimeDisplay->setReadOnly(true);
	m_TimeDisplay->setEnabled(false);
	m_TimeDisplay->setFixedWidth(60);
	m_TimeDisplay->setAlignment(Qt::AlignCenter);

	/* Framerate */
	m_FramerateBox = new QComboBox;
	m_FramerateBox->setEditable(true);
	/* Setup the Focus policy to only accept focus when clicked on */
	m_FramerateBox->setFocusPolicy(Qt::ClickFocus);

	/* Add to options layout */
	/* Layout spacing */
	optionsLayout->setSpacing(0);

	optionsLayout->addWidget(m_FramerateBox);
	optionsLayout->insertStretch(1, 1);

	optionsLayout->addWidget(m_TimeDisplay);
	optionsLayout->insertStretch(3, 1);

	/* Buttons */
	optionsLayout->addWidget(m_StartFrameButton);
	optionsLayout->addWidget(m_PrevFrameButton);
	optionsLayout->addWidget(m_BackwardButton);
	optionsLayout->addWidget(m_StopButton);
	optionsLayout->addWidget(m_ForwardButton);
	optionsLayout->addWidget(m_NextFrameButton);
	optionsLayout->addWidget(m_EndFrameButton);

	/* Timeslider */
	m_Timeslider = new QSlider(Qt::Horizontal);

	/* Add to the main Layout */
	layout->addLayout(optionsLayout);
	layout->addWidget(m_Timeslider);

	/* Spacing */
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
}

void Timeslider::Stylize()
{
	std::string timesliderSS = "\
			QSlider::groove:horizontal {\
				border: 1px solid #999999;\
				height: 10px;\
			}\
			QSlider::handle:horizonatal{\
				background: #fff;\
				width: 10px;\
				margin: -1px -1px;\
				border: 1px solid #2a2a2a;\
			}\
			QSlider::add-page:horizontal{\
				background: #c4c4c4;\
			}\
			QSlider::sub-page:horizontal{\
				background: #b2cf53;\
			}\
		";

	m_Timeslider->setStyleSheet(timesliderSS.c_str());
}

void Timeslider::Connect()
{
	connect(m_Timeslider, &QSlider::valueChanged, this, &Timeslider::TimeUpdated);
	//connect(m_ForwardButton, &QPushButton::clicked, this, &Timeslider::PlayedForwards);
	//connect(m_BackwardButton, &QPushButton::clicked, this, &Timeslider::PlayedForwards);
	//connect(m_ForwardButton, &QPushButton::clicked, this, [this]() { emit Played(PlayState::FORWARDS); });
	//connect(m_BackwardButton, &QPushButton::clicked, this, [this]() { emit Played(PlayState::BACKWARDS); });

	connect(m_ForwardButton, &QPushButton::clicked, this, [this]() { Stop(); m_ForwardsTimer->start(1000 / Framerate()); });
	connect(m_BackwardButton, &QPushButton::clicked, this, [this]() { Stop(); m_BackwardsTimer->start(1000 / Framerate()); });

	connect(m_ForwardsTimer, &QTimer::timeout, this, [this]() { Play(Timeslider::PlayState::FORWARDS); });
	connect(m_BackwardsTimer, &QTimer::timeout, this, [this]() { Play(Timeslider::PlayState::BACKWARDS); });

	connect(m_StopButton, &QPushButton::clicked, this, &Timeslider::Stop);
	connect(m_NextFrameButton, &QPushButton::clicked, this, &Timeslider::NextFrame);
	connect(m_PrevFrameButton, &QPushButton::clicked, this, &Timeslider::PreviousFrame);

	connect(m_EndFrameButton, &QPushButton::clicked, this, [this]() { m_Timeslider->setValue(m_Timeslider->maximum()); });
	connect(m_StartFrameButton, &QPushButton::clicked, this, [this]() { m_Timeslider->setValue(m_Timeslider->minimum()); });
}

void Timeslider::Setup()
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

	m_FramerateBox->addItems(values);
	m_FramerateBox->setValidator(m_DoubleValidator);

	// Set the default rate
	SetFramerate("24");

	SetFrame(m_Timeslider->minimum());

	/* Fixed Height for the timeslider panel */
	setFixedHeight(50);
}

double Timeslider::Framerate() const
{
	// Fetch the current value from Framerate box
	return m_FramerateBox->currentText().toDouble();
}

int Timeslider::Frame() const
{
	return m_Timeslider->value();
}

int Timeslider::Minimum() const
{
	return m_Timeslider->minimum();
}

int Timeslider::Maximum() const
{
	return m_Timeslider->maximum();
}

void Timeslider::SetFramerate(const double rate)
{
	m_FramerateBox->setCurrentText(std::to_string(rate).c_str());
}

void Timeslider::SetFramerate(const std::string& rate)
{
	m_FramerateBox->setCurrentText(rate.c_str());
}

void Timeslider::SetFrame(const int frame)
{
	m_TimeDisplay->setText(std::to_string(frame).c_str());
	m_Timeslider->setValue(frame);
}

void Timeslider::TimeUpdated(const int time)
{
	m_TimeDisplay->setText(std::to_string(time).c_str());
	emit TimeChanged(time);
}

void Timeslider::SetMaximum(const int frame)
{
	m_Timeslider->setMaximum(frame);
}

void Timeslider::SetMinimum(const int frame)
{
	m_Timeslider->setMinimum(frame);
}

void Timeslider::SetRange(const int min, const int max)
{
	m_Timeslider->setRange(min, max);
}

void Timeslider::PlayForwards()
{
	m_ForwardsTimer->start(1000 / Framerate());
}

void Timeslider::PlayBackwards()
{
	m_BackwardsTimer->start(1000 / Framerate());
}

void Timeslider::Stop()
{
	m_ForwardsTimer->stop();
	m_BackwardsTimer->stop();
}

void Timeslider::NextFrame()
{
	int currentFrame = m_Timeslider->value();

	if (currentFrame < m_Timeslider->maximum())
		m_Timeslider->setValue(currentFrame + 1);
	else
		m_Timeslider->setValue(m_Timeslider->minimum());
}

void Timeslider::PreviousFrame()
{
	int currentFrame = m_Timeslider->value();

	if (currentFrame == m_Timeslider->minimum())
		m_Timeslider->setValue(m_Timeslider->maximum());
	else
		m_Timeslider->setValue(currentFrame - 1);
}

void Timeslider::Play(const Timeslider::PlayState& state)
{
	if (state == Timeslider::PlayState::FORWARDS)
	{
		NextFrame();
	}
	else if (state == Timeslider::PlayState::BACKWARDS)
	{
		PreviousFrame();
	}
}

VOID_NAMESPACE_CLOSE
