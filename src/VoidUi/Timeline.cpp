/* STD */
#include <algorithm>

/* Internal */
#include "Timeline.h"

/* Qt */
#include <QIcon>
#include <QMouseEvent>
#include <QLayout>
#include <QMenu>
#include <QPainter>
#include <QStyle>
#include <QValidator>

static const int BUTTON_WIDTH = 30;
static const int SMALL_BUTTON_WIDTH = 20;
static const int MEDIUM_BUTTON_WIDTH = 40;

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
	, m_UserStartframe(0)
	, m_UserEndframe(0)
{
	/* Fixed height */
	setFixedHeight(SL_HEIGHT);

	/* Enable mouse tracking to be able to show the frame being hovered on */
	setMouseTracking(true);
}

Timeslider::~Timeslider()
{
}

void Timeslider::enterEvent(EnterEvent* event)
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

	/* Pos for Framing User Defined Frame */
	int startpos = 0, endpos = 0;

	/* Width of each unit value represented in the slider */
	double uwidth = double(width()) / (maximum() - minimum());

	/* Groove {{{ */
	painter.setPen(QColor(30, 30, 30));
	painter.setBrush(QBrush(QColor(30, 30, 30)));

	/* Base track color */
	painter.fillRect(rect(), Qt::black);

	/**
	 * This rect will have a slightly lighter shade to indicate playing range
	 * This range is obviously affected by the user defined in-out set
	 * making the overall playable range more prominent and visible to the user
	 */
	QRect r(rect());

	/* Draw In and Out Frames with Red if they have been defined {{{ */
	if (m_UserStartframe)
	{
		/* Position */
		startpos = (m_UserStartframe - minimum()) * uwidth;

		r.setLeft(startpos);
	}

	if (m_UserEndframe)
	{
		/* Position */
		endpos = (m_UserEndframe - minimum()) * uwidth;

		/* Set the right most position */
		r.setRight(endpos);
	}
	/* }}} */

	// painter.drawRect(r);
	painter.fillRect(r, QColor(30, 30, 30));

	/* Position Handle */
	int hpos = width() * (value() - minimum()) / std::max((maximum() - minimum()), 1);
	painter.setBrush(SL_HANDLE_COLOR);

	/* The position handle should have a minimum width */
	int hwidth = std::max(uwidth, 4.0);

	painter.drawRect(hpos - hwidth / 2, 0, hwidth, height());
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
	int number = std::max(range / SL_MARKING_STEP, 1);	// ensure that we don't divide by zero

	/* Step here would give the step based on the number of markings are being generated */
	int step = range / number;

	for (int i = minimum(); i < maximum() ; i+= step)
	{
		int pos = width() * (i - minimum()) / range;
		/* Draw Line representing Marked frames on the timeslider */
		painter.drawLine(pos, height() - 10, pos, height());
	}

	/* Draw In and Out Frames with Red if they have been defined {{{ */
	if (startpos)
	{
		/* Draw a Line to indicate that this is a marked frame */
		painter.setPen(QPen(QColor(200, 75, 60), 2));
		painter.drawLine(startpos, 0, startpos, height());

		/* Draw Text to indicate the frame */
		painter.drawText(startpos + 4, height() / 2, QString::number(m_UserStartframe));
	}

	if (endpos)
	{
		/* Draw a Line to indicate that this is a marked frame */
		painter.setPen(QPen(QColor(200, 75, 60), 2));
		painter.drawLine(endpos, 0, endpos, height());

		/* Draw Text to indicate the frame */
		painter.drawText(endpos + 4, height() / 2, QString::number(m_UserEndframe));
	}
	/* }}} */

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

void Timeslider::SetUserFirstframe(int frame)
{
	/* Set the user defined first frame */
	m_UserStartframe = frame;

	/* Repaint the timeslider -- for any denotions of the frame */
	update();
}

void Timeslider::SetUserEndframe(int frame)
{
	/* Set the user defined last frame */
	m_UserEndframe = frame;

	/* Repaint the timeslider -- for any denotions of the frame */
	update();
}

void Timeslider::ResetRange()
{
	/* Reset the frames */
	m_UserStartframe = 0;
	m_UserEndframe = 0;

	/* Repaint the timeslider -- for clearing any denotions of the frame */
	update();
}

void Timeslider::ResetStartFrame()
{
	/* Reset the start frame */
	m_UserStartframe = 0;

	/* Repaint the timeslider -- for clearing any denotions of the frame */
	update();
}

void Timeslider::ResetEndFrame()
{
	/* Reset the end frame */
	m_UserEndframe = 0;

	/* Repaint the timeslider -- for clearing any denotions of the frame */
	update();
}

int Timeslider::Minimum()
{
	/* If a user start frame is defined -> return that */
	if (m_UserStartframe)
		return m_UserStartframe;

	/* Else return the base minimum frame */
	return minimum();
}

int Timeslider::Maximum()
{
	/* If a user end frame is defined -> return that */
	if (m_UserEndframe)
		return m_UserEndframe;

	/* Else return the base maximum frame */
	return maximum();
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

	/* Repaint after a frame has been cached to redraw the cache line */
	update();
}

void Timeslider::ClearCachedFrames()
{
	/* Clears the contents of the cached frames */
	m_CachedFrames.clear();

	/* Repaint after the cache frames have been cleared */
	update();
}

/* Loop Type Button {{{ */

LoopTypeButton::LoopTypeButton(QWidget* parent)
	: QPushButton(parent)
	, m_LoopType(LoopType::LoopInfinitely)
{
	/* Update Loop States to be used on the button */
	m_LoopState[LoopType::LoopInfinitely] = {"Loop Infinitely", "resources/icons/icon_repeat.svg"};
	m_LoopState[LoopType::PingPong] = {"Bounce", "resources/icons/icon_bounce.svg"};
	m_LoopState[LoopType::PlayOnce] = {"Play Once", "resources/icons/icon_arrow_right.svg"};

	/* Update to set the state on the button */
	Build();

	/* No Visible borders */
	setFlat(true);
}

LoopTypeButton::~LoopTypeButton()
{
	/* Delete the Menu and all it's children */
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

	for (std::pair<LoopType, LoopState> entry: m_LoopState)
	{
		/* Action for the menu */
		QAction* action = new QAction(entry.second.text.c_str(), m_Menu);

		/* Connect the action to set the Loop State */
		connect(action, &QAction::triggered, this, [this, entry]() { SetLoopType(entry.first); });

		/* Add to the Menu */
		m_Menu->addAction(action);
	}

	/* Set the Menu on the button */
	setMenu(m_Menu);
}

/* }}} */

Timeline::Timeline(QWidget* parent)
	: QWidget(parent)
	, m_Start(0)
	, m_End(0)
	, m_LoopType(LoopType::LoopInfinitely)
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
	QHBoxLayout* playoptsLayout = new QHBoxLayout;

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
	#if _QT6 		/* Qt6 Compat */
	QPixmap p = s->standardPixmap(s->SP_MediaPlay);
	QImage backImg(p.toImage().flipped(Qt::Horizontal));
	#else
	QPixmap p = s->standardPixmap(s->SP_MediaPlay);
	QImage backImg(p.toImage().mirrored(true, false));
	#endif
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

	/* In - Out Framing */
	m_InFrameButton = new ToggleStatePushButton("I");
	m_InFrameButton->setFixedWidth(SMALL_BUTTON_WIDTH);

	m_OutFrameButton = new ToggleStatePushButton("O");
	m_OutFrameButton->setFixedWidth(SMALL_BUTTON_WIDTH);

	/* Loop Type Button */
	m_LoopTypeButton = new LoopTypeButton;
	m_LoopTypeButton->setFixedWidth(MEDIUM_BUTTON_WIDTH);

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
	optionsLayout->setSpacing(10);
	playoptsLayout->setSpacing(0);

	optionsLayout->addWidget(m_FramerateBox);
	optionsLayout->insertStretch(1, 1);

	optionsLayout->addWidget(m_TimeDisplay);
	optionsLayout->insertStretch(3, 1);

	/* Buttons */
	optionsLayout->addWidget(m_LoopTypeButton);
	optionsLayout->addWidget(m_InFrameButton);

	playoptsLayout->addWidget(m_StartFrameButton);
	playoptsLayout->addWidget(m_PrevFrameButton);
	playoptsLayout->addWidget(m_BackwardButton);
	playoptsLayout->addWidget(m_StopButton);
	playoptsLayout->addWidget(m_ForwardButton);
	playoptsLayout->addWidget(m_NextFrameButton);
	playoptsLayout->addWidget(m_EndFrameButton);

	optionsLayout->addLayout(playoptsLayout);

	optionsLayout->addWidget(m_OutFrameButton);

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

	connect(m_InFrameButton, &QPushButton::clicked, this, &Timeline::ResetInFrame);
	connect(m_OutFrameButton, &QPushButton::clicked, this, &Timeline::ResetOutFrame);

	connect(m_LoopTypeButton, &LoopTypeButton::loopTypeChanged, this, [this](const LoopType& looptype) { m_LoopType = looptype; });
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

	/* Update the internal range */
	m_Start = m_Timeslider->minimum();
	m_End = m_Timeslider->maximum();
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
	/* Reset any user defined range for the timeslider */
	m_Timeslider->ResetRange();

	/* Update timeslider range */
	m_Timeslider->setRange(min, max);

	/* Update internal range */
	m_Start = min;
	m_End = max;
}

void Timeline::ResetRange()
{
	/* Reset the range for the timeslider */
	m_Timeslider->ResetRange();

	/**
	 * And update the internal range to now refer those values
	 * At this point the timeslider does not have any user defined values
	 * So we could just query the base minimum and maximum values from it
	 */
	m_Start = m_Timeslider->minimum();
	m_End = m_Timeslider->maximum();

	/* Reset the Toggle button state */
	m_InFrameButton->Toggle(false);
	m_OutFrameButton->Toggle(false);
}

void Timeline::SetUserFirstframe(int frame)
{
	/* Check if the end frame is lesser than the provided start frame */
	if (m_Timeslider->m_UserEndframe && (frame > m_Timeslider->m_UserEndframe))
	{
		/* If so -> Reset the end frame */
		m_Timeslider->m_UserEndframe = 0;
		/* Update internal end frame */
		m_End = m_Timeslider->maximum();
	}

	/* Update the first user frame on the timeslider */
	m_Timeslider->SetUserFirstframe(frame);

	/* Update the start frame */
	m_Start = frame;
}

void Timeline::SetUserEndframe(int frame)
{
	/* Check if the start frame is greater than the provided end frame */
	if (m_Timeslider->m_UserStartframe && (frame < m_Timeslider->m_UserStartframe))
	{
		/* If so -> Reset the start frame */
		m_Timeslider->m_UserStartframe = 0;
		/* Update internal start frame */
		m_Start = m_Timeslider->minimum();
	}

	/* Update the last user frame on the timeslider */
	m_Timeslider->SetUserEndframe(frame);

	/* Update the end frame */
	m_End = frame;
}

void Timeline::ResetInFrame()
{
	/* Fetch the current frame */
	int frame = Frame();

	/* Check if it's already our start frame */
	if (frame == m_Start)
	{
		m_Timeslider->ResetStartFrame();
		m_Start = m_Timeslider->minimum();
	}
	else
	{
		/* Update the Start frame with the current frame */
		SetUserFirstframe(frame);
	}

	/* If the User start frame has been set -> Reflect the same on the Toggle Push button */
	m_InFrameButton->Toggle(static_cast<bool>(m_Timeslider->m_UserStartframe));
}

void Timeline::ResetOutFrame()
{
	/* Fetch the current frame */
	int frame = Frame();

	/* Check if it's already our end frame */
	if (frame == m_End)
	{
		m_Timeslider->ResetEndFrame();
		m_End = m_Timeslider->maximum();
	}
	else
	{
		/* Update the End frame with the current frame */
		SetUserEndframe(frame);
	}

	/* If the User end frame has been set -> Reflect the same on the Toggle Push button */
	m_OutFrameButton->Toggle(static_cast<bool>(m_Timeslider->m_UserEndframe));
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

	if (currentFrame < m_End)
		m_Timeslider->setValue(currentFrame + 1);
	else
		m_Timeslider->setValue(m_Start);
}

void Timeline::PreviousFrame()
{
	int currentFrame = m_Timeslider->value();

	if (currentFrame == m_Start)
		m_Timeslider->setValue(m_End);
	else
		m_Timeslider->setValue(currentFrame - 1);
}

void Timeline::PlayNextFrame()
{
	int currentFrame = m_Timeslider->value();

	if (currentFrame < m_End)
	{
		m_Timeslider->setValue(currentFrame + 1);
	}
	else
	{
		/**
		 * This is effectively the end of the timeline
		 * What happens next depends on the loop type
		 */
		if (m_LoopType == LoopType::PlayOnce)
		{
			/* Stop Playing now and return */
			Stop();
			return;
		}
		else if (m_LoopType == LoopType::PingPong)
		{
			/* We're in a Ping Pong Mode so start playing backwards if we're here */
			PlayBackwards();
			return;
		}

		/* Else we set the first frame on the timeline and let it continue till the user decides to stop, or close the player :D */
		m_Timeslider->setValue(m_Start);
	}
}

void Timeline::PlayPreviousFrame()
{
	int currentFrame = m_Timeslider->value();

	if (currentFrame == m_Start)
	{
		/**
		 * This is effectively the start of the timeline (since we're playing backwards)
		 * What happens next depends on the loop type
		 */
		if (m_LoopType == LoopType::PlayOnce)
		{
			/* Stop Playing now and return */
			Stop();
			return;
		}
		else if (m_LoopType == LoopType::PingPong)
		{
			/* We're in a Ping Pong Mode so start playing forwards if we're here */
			PlayForwards();
			return;
		}
	
		/* Else we set the last frame on the timeline and let it continue till the user decides to stop, or close the player :D */
		m_Timeslider->setValue(m_End);
	}
	else
	{
		m_Timeslider->setValue(currentFrame - 1);
	}
}

void Timeline::Play(const Timeline::PlayState& state)
{
	/* Check if the current playhead is in playable range */
	if (!m_Timeslider->InRange(m_Timeslider->value()))
	{
		/**
		 * Move the playhead to the beginning of the playable range
		 * Ideally the Internal start should always have the playable range on it
		 */
		m_Timeslider->setValue(m_Start);
	}

	/* once the playhead is placed correctly -> We can begin playing */
	if (state == Timeline::PlayState::FORWARDS)
	{
		PlayNextFrame();
	}
	else if (state == Timeline::PlayState::BACKWARDS)
	{
		PlayPreviousFrame();
	}
}

VOID_NAMESPACE_CLOSE
