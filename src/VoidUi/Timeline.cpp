/* STD */
#include <algorithm>

/* Internal */
#include "Timeline.h"

/* Qt */
#include <QIcon>
#include <QMouseEvent>
#include <QLayout>
#include <QPainter>
#include <QStyle>
#include <QValidator>

static const int BUTTON_WIDTH = 30;

/* Timeslider Markings Step */
static const int SL_MARKING_STEP = 5;

/* Timeslider Fixed height */
static const int SL_HEIGHT = 30;

/* Color for the handle of the timeslider */
static const QColor SL_HANDLE_COLOR = {160, 190, 60};

/* Color for the frame displayed on the timeslider */
static const QColor SL_FRAME_COLOR = {190, 150, 60};

/* Color for the cache denotion of the timeslider */
static const QColor SL_CACHE_COLOR = {190, 150, 60};

VOID_NAMESPACE_OPEN

Timeslider::Timeslider(Qt::Orientation orientation, QWidget *parent)
	: QSlider(orientation, parent)
	, m_Focussed(false)
	, m_HovXPos(0)
	, m_HoveredFrame(0)
{
	/* Fixed height */
	setFixedHeight(SL_HEIGHT);

	/* Enable mouse tracking to be able to show the frame being hovered on */
	setMouseTracking(true);
}

Timeslider::~Timeslider()
{
}

void Timeslider::enterEvent(QEvent* event)
{
	m_Focussed = true;
}

void Timeslider::leaveEvent(QEvent* event)
{
	m_Focussed = false;

	/* Update the Hovered details */
	m_HoveredFrame = -1;
	m_HovXPos = -1;

	/* Trigger a repaint */
	update();
}

void Timeslider::mouseMoveEvent(QMouseEvent* event)
{
	/* Allow other behaviour */
	QSlider::mouseMoveEvent(event);

	if (m_Focussed)
	{
		/* Update the hovered information */
		UpdateHovered(event->pos().x());
	}

	/* Trigger a repaint */
	update();
}

void Timeslider::mousePressEvent(QMouseEvent* event)
{
	m_Focussed = false;
	
	/* Fetch the value from postion of the mouse press */
	int value = QStyle::sliderValueFromPosition(
					minimum(),
					maximum(),
					event->pos().x(),
					width(),
					orientation() == Qt::Vertical
				);

				/* Set the value on the slider */
				setValue(value);
				
	/* Allow dragging behaviour */
	QSlider::mousePressEvent(event);

	/* Repaint */
	update();
}

void Timeslider::mouseReleaseEvent(QMouseEvent* event)
{
	if (m_HovXPos != -1)
	{
		/* Focus Back on the Timeslider */
		m_Focussed = true;

		/* Update the hovered information */
		UpdateHovered(event->pos().x());
	}

	/* Default Behaviour */
	QSlider::mouseReleaseEvent(event);

	/* Repaint */
	update();
}

void Timeslider::paintEvent(QPaintEvent* event)
{
	/* Painter to draw components for the Timeslider */
	QPainter painter(this);

	/* Width of each unit value represented in the slider */
	double uwidth = double(width()) / (maximum() - minimum());

	/* Groove {{{ */
	painter.setPen(QColor(30, 30, 30));
	painter.setBrush(QBrush(QColor(30, 30, 30)));

	/* Base track color */
	painter.drawRect(0, 0, width(), height());

	/* Position Handle */
	int hpos = width() * (value() - minimum()) / (maximum() - minimum());
	painter.setBrush(SL_HANDLE_COLOR);
	painter.drawRect(hpos - uwidth / 2, 0, uwidth, height());
	/* }}} */

	/* Painter for drawing the Cached frame {{{ */
	painter.setPen(QPen(SL_CACHE_COLOR, 3));

	for (int frame: m_CachedFrames)
	{
		/* Position */
		int xpos = (frame - minimum()) * uwidth;
		/* Draw line representing the frame which has been cached */
		painter.drawLine(xpos, 0, xpos + uwidth, 0);
	}
	/* }}} */

	/* Update pen to draw Pre-marked frames on the timeslider */
	painter.setPen(QPen(Qt::gray, 2));

	/* Draw Lines */
	int range = maximum() - minimum();

	/* Number of frames to be drawn */
	int number = range / SL_MARKING_STEP;

	/* Step here would give the step based on the number of markings are being generated */
	int step = range / number;

	for (int i = minimum(); i < maximum() ; i+= step)
	{
		int pos = width() * (i - minimum()) / range;
		/* Draw Line representing Marked frames on the timeslider */
		painter.drawLine(pos, height() - 10, pos, height());
	}

	/* This is the part where we draw the focussed frame on the timeslider when mouse is hovered {{{ */
	if (m_Focussed)
	{
		/* Draw the frame number */
		painter.setPen(SL_FRAME_COLOR);
		painter.drawText(m_HovXPos - uwidth / 2, height() / 2, QString::number(m_HoveredFrame));
	}
	/* }}} */
}

void Timeslider::UpdateHovered(int xpos)
{
	/* Set the Hovered position based on the x position of the event click */
	m_HovXPos = xpos;
	
	/* Fetch the slider value based on the position */
	m_HoveredFrame = QStyle::sliderValueFromPosition(
						minimum(),
						maximum(),
						m_HovXPos,
						width(),
						orientation() == Qt::Vertical
					);
}

void Timeslider::AddCacheFrame(int frame)
{
	std::vector<int>::iterator it = std::find(m_CachedFrames.begin(), m_CachedFrames.end(), frame);

	/*
	 * If the value is not present in cached frames already
	 * Add it to the set
	 */
	if (it == m_CachedFrames.end())
	{
		m_CachedFrames.push_back(frame);
	}
}

void Timeslider::ClearCachedFrames()
{
	/* Clears the contents of the cached frames */
	m_CachedFrames.clear();
}

Timeline::Timeline(QWidget* parent)
	: QWidget(parent)
{
	/* Build the layout of the widget */
	Build();

	/* Setup any values */
	Setup();

	/* Connect Signals */
	Connect();
}

Timeline::~Timeline()
{
}

void Timeline::Build()
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
	m_Timeslider = new Timeslider(Qt::Horizontal);

	/* Add to the main Layout */
	layout->addLayout(optionsLayout);
	layout->addWidget(m_Timeslider);

	/* Spacing */
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
}

void Timeline::Connect()
{
	connect(m_Timeslider, &QSlider::valueChanged, this, &Timeline::TimeUpdated);

	connect(m_ForwardButton, &QPushButton::clicked, this, [this]() { Stop(); m_ForwardsTimer->start(1000 / Framerate()); });
	connect(m_BackwardButton, &QPushButton::clicked, this, [this]() { Stop(); m_BackwardsTimer->start(1000 / Framerate()); });

	connect(m_ForwardsTimer, &QTimer::timeout, this, [this]() { Play(Timeline::PlayState::FORWARDS); });
	connect(m_BackwardsTimer, &QTimer::timeout, this, [this]() { Play(Timeline::PlayState::BACKWARDS); });

	connect(m_StopButton, &QPushButton::clicked, this, &Timeline::Stop);
	connect(m_NextFrameButton, &QPushButton::clicked, this, &Timeline::NextFrame);
	connect(m_PrevFrameButton, &QPushButton::clicked, this, &Timeline::PreviousFrame);

	connect(m_StartFrameButton, &QPushButton::clicked, this, &Timeline::MoveToStart);
	connect(m_EndFrameButton, &QPushButton::clicked, this, &Timeline::MoveToEnd);
}

void Timeline::Setup()
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

void Timeline::SetFramerate(const double rate)
{
	m_FramerateBox->setCurrentText(std::to_string(rate).c_str());
}

void Timeline::SetFramerate(const std::string& rate)
{
	m_FramerateBox->setCurrentText(rate.c_str());
}

void Timeline::SetFrame(const int frame)
{
	m_TimeDisplay->setText(std::to_string(frame).c_str());
	m_Timeslider->setValue(frame);
}

void Timeline::TimeUpdated(const int time)
{
	m_TimeDisplay->setText(std::to_string(time).c_str());
	emit TimeChanged(time);
}

void Timeline::SetRange(const int min, const int max)
{
	m_Timeslider->setRange(min, max);
}

void Timeline::PlayForwards()
{
	/* Stop Before we start the timer */
	Stop();

	m_ForwardsTimer->start(1000 / Framerate());
}

void Timeline::PlayBackwards()
{
	/* Stop Before we start the timer */
	Stop();

	m_BackwardsTimer->start(1000 / Framerate());
}

void Timeline::Stop()
{
	m_ForwardsTimer->stop();
	m_BackwardsTimer->stop();
}

void Timeline::NextFrame()
{
	int currentFrame = m_Timeslider->value();

	if (currentFrame < m_Timeslider->maximum())
		m_Timeslider->setValue(currentFrame + 1);
	else
		m_Timeslider->setValue(m_Timeslider->minimum());
}

void Timeline::PreviousFrame()
{
	int currentFrame = m_Timeslider->value();

	if (currentFrame == m_Timeslider->minimum())
		m_Timeslider->setValue(m_Timeslider->maximum());
	else
		m_Timeslider->setValue(currentFrame - 1);
}

void Timeline::Play(const Timeline::PlayState& state)
{
	if (state == Timeline::PlayState::FORWARDS)
	{
		NextFrame();
	}
	else if (state == Timeline::PlayState::BACKWARDS)
	{
		PreviousFrame();
	}
}

VOID_NAMESPACE_CLOSE
