// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <algorithm>

/* Qt */
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QIcon>
#include <QLayout>
#include <QStyle>

/* Internal */
#include "Timeline.h"
#include "VoidUi/Engine/IconForge.h"
#include "VoidUi/QExtensions/Tooltip.h"

static const int BUTTON_WIDTH = 30;
static const int SMALL_BUTTON_WIDTH = 20;
static const int MEDIUM_BUTTON_WIDTH = 40;

VOID_NAMESPACE_OPEN

Timeline::Timeline(QWidget* parent)
	: QWidget(parent)
	, m_Start(0)
	, m_End(0)
	, m_LoopType(LoopType::LoopInfinitely)
	, m_Playing(false)
	, m_Playstate(PlayState::STOPPED)
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
	Stop();

	m_Layout->deleteLater();
	delete m_Layout;
	m_Layout = nullptr;
}

void Timeline::Build()
{
	/* Base Layout */
	m_Layout = new QVBoxLayout(this);

	/* Left Side Layout */
	m_LeftLayout = new QHBoxLayout;
	m_RightLayout = new QHBoxLayout;

	QHBoxLayout* optionsLayout = new QHBoxLayout;

	QStyle* s = style();

	/* Forwards */
	m_ForwardButton = new QPushButton;
	m_ForwardButton->setIcon(IconForge::GetIcon(IconType::icon_play_arrow, _DARK_COLOR(QPalette::Text, 100)));
	m_ForwardButton->setFixedWidth(BUTTON_WIDTH);
	m_ForwardButton->setToolTip(ToolTipString("Play Forwards", "Starts Playing in forward direction.").c_str());

	m_NextFrameButton = new QPushButton;
	m_NextFrameButton->setIcon(IconForge::GetIcon(IconType::icon_fast_forward, _DARK_COLOR(QPalette::Text, 100)));
	m_NextFrameButton->setFixedWidth(BUTTON_WIDTH);
	m_NextFrameButton->setToolTip(ToolTipString("Next Frame", "Moves to the next frame.").c_str());

	m_EndFrameButton = new QPushButton;
	m_EndFrameButton->setIcon(IconForge::GetIcon(IconType::icon_skip_next, _DARK_COLOR(QPalette::Text, 100)));
	m_EndFrameButton->setFixedWidth(BUTTON_WIDTH);
	m_EndFrameButton->setToolTip(ToolTipString("End Frame", "Moves to the last frame.").c_str());

	/* Backwards */
	m_BackwardButton = new QPushButton;
	m_BackwardButton->setIcon(IconForge::GetIcon(IconType::icon_arrow_back, _DARK_COLOR(QPalette::Text, 100)));
	m_BackwardButton->setFixedWidth(BUTTON_WIDTH);
	m_BackwardButton->setToolTip(ToolTipString("Play Backwards", "Starts Playing in reverse direction.").c_str());

	m_PrevFrameButton = new QPushButton;
	m_PrevFrameButton->setIcon(IconForge::GetIcon(IconType::icon_fast_rewind,  _DARK_COLOR(QPalette::Text, 100)));
	m_PrevFrameButton->setFixedWidth(BUTTON_WIDTH);
	m_PrevFrameButton->setToolTip(ToolTipString("Previous Frame", "Moves to the previous frame.").c_str());

	m_StartFrameButton = new QPushButton;
	m_StartFrameButton->setIcon(IconForge::GetIcon(IconType::icon_skip_previous,  _DARK_COLOR(QPalette::Text, 100)));
	m_StartFrameButton->setFixedWidth(BUTTON_WIDTH);
	m_StartFrameButton->setToolTip(ToolTipString("Start Frame", "Moves to the first frame.").c_str());

	/* Stop and others */
	m_StopButton = new QPushButton;
	m_StopButton->setIcon(IconForge::GetIcon(IconType::icon_pause, _DARK_COLOR(QPalette::Text, 100)));
	m_StopButton->setFixedWidth(BUTTON_WIDTH);
	m_StopButton->setToolTip(ToolTipString("Stop", "Stops playing the media.").c_str());

	/* In - Out Framing */
	m_InFrameButton = new ToggleStatePushButton("I");
	m_InFrameButton->setFixedWidth(SMALL_BUTTON_WIDTH);
	m_InFrameButton->setToolTip(ToolTipString("Set In frame", "Sets the current frame as the new inframe for playing.").c_str());

	m_OutFrameButton = new ToggleStatePushButton("O");
	m_OutFrameButton->setFixedWidth(SMALL_BUTTON_WIDTH);
	m_OutFrameButton->setToolTip(ToolTipString("Set Out frame", "Sets the current frame as the out inframe for playing.").c_str());

	/* Loop Type Button */
	m_LoopTypeButton = new LoopTypeButton;
	m_LoopTypeButton->setFixedWidth(MEDIUM_BUTTON_WIDTH);
	m_LoopTypeButton->setToolTip(ToolTipString("Playback Loop", "Sets the loop for playback.").c_str());

	/* Fullscreen Button */
	m_FullscreenButton = new QPushButton;
	m_FullscreenButton->setIcon(IconForge::GetIcon(IconType::icon_pageless, _DARK_COLOR(QPalette::Text, 100)));
	m_FullscreenButton->setFlat(true);
	m_FullscreenButton->setFixedWidth(SMALL_BUTTON_WIDTH);
	m_FullscreenButton->setToolTip(ToolTipString("Fullscreen", "Plays media in fullscreen.").c_str());

	/* Timing */
	m_TimeDisplay = new TimeDisplay;

	/* Framerate */
	m_FramerateBox = new FramerateBox;

	m_LeftLayout->addWidget(m_FramerateBox);
	m_LeftLayout->addWidget(m_LoopTypeButton);
	
	/* Spacer */
	m_LeftLayout->addStretch(1);

	/* Spacer */
	m_RightLayout->addStretch(1);

	m_RightLayout->setSpacing(2);
	m_RightLayout->addWidget(m_InFrameButton);
	m_RightLayout->addWidget(m_StartFrameButton);
	m_RightLayout->addWidget(m_PrevFrameButton);

	m_RightLayout->addWidget(m_TimeDisplay);

	m_RightLayout->addWidget(m_NextFrameButton);
	m_RightLayout->addWidget(m_EndFrameButton);
	m_RightLayout->addWidget(m_OutFrameButton);

	m_RightLayout->addItem(new QSpacerItem(40, 0, QSizePolicy::Minimum, QSizePolicy::Minimum));

	m_RightLayout->addWidget(m_FullscreenButton);
	
	optionsLayout->addLayout(m_LeftLayout);

	optionsLayout->addWidget(m_BackwardButton);
	optionsLayout->addWidget(m_StopButton);
	optionsLayout->addWidget(m_ForwardButton);

	optionsLayout->setSpacing(2);
	optionsLayout->addLayout(m_RightLayout);

	/* Timeslider */
	m_Timeslider = new Timeslider(Qt::Horizontal);

	/* Add to the main Layout */
	m_Layout->addLayout(optionsLayout);
	m_Layout->addWidget(m_Timeslider);

	/* Spacing */
	m_Layout->setSpacing(0);
	m_Layout->setContentsMargins(0, 0, 0, 0);
}

void Timeline::Connect()
{
	connect(m_Timeslider, &QSlider::valueChanged, this, &Timeline::TimeUpdated, Qt::DirectConnection);

	connect(m_ForwardButton, &QPushButton::clicked, this, [this]() { Play(Timeline::PlayState::FORWARDS); });
	connect(m_BackwardButton, &QPushButton::clicked, this, [this]() { Play(Timeline::PlayState::BACKWARDS); });

	connect(&m_PlayTimer, &QTimer::timeout, this, &Timeline::TimerPlaybackLoop, Qt::DirectConnection);

	connect(m_StopButton, &QPushButton::clicked, this, &Timeline::Stop);
	connect(m_NextFrameButton, &QPushButton::clicked, this, &Timeline::NextFrame);
	connect(m_PrevFrameButton, &QPushButton::clicked, this, &Timeline::PreviousFrame);

	connect(m_StartFrameButton, &QPushButton::clicked, this, &Timeline::MoveToStart);
	connect(m_EndFrameButton, &QPushButton::clicked, this, &Timeline::MoveToEnd);

	connect(m_InFrameButton, &QPushButton::clicked, this, &Timeline::ResetInFrame);
	connect(m_OutFrameButton, &QPushButton::clicked, this, &Timeline::ResetOutFrame);

	connect(m_LoopTypeButton, &LoopTypeButton::loopTypeChanged, this, [this](const LoopType& looptype) { m_LoopType = looptype; });
	connect(m_FullscreenButton, &QPushButton::clicked, this, &Timeline::fullscreenRequested);

	connect(m_FramerateBox, &FramerateBox::framerateChanged, this, [this](double) { Replay(); });
}

void Timeline::Setup()
{
	/* Set the default rate */
	SetFramerate("24");

	SetFrame(m_Timeslider->minimum());

	/* Fixed Height for the timeslider panel */
	setFixedHeight(50);

	/* Update the internal range */
	m_Start = m_Timeslider->minimum();
	m_End = m_Timeslider->maximum();
}

void Timeline::StartPlayback()
{
	m_FrameInterval = 1000 / Framerate();

	// bool expected = false;
	// if (!m_Playing.compare_exchange_strong(expected, true))
	// {
	// 	return;
	// }
	// /* Start Playback worker async */
	// m_Worker = std::async(std::launch::async, &Timeline::PlaybackLoop, this);

	m_PlayTimer.start(m_FrameInterval);
}

void Timeline::PlaybackLoop()
{
	QElapsedTimer timer;
	timer.start();

	while(m_Playing)
	{
		if (timer.elapsed() >= m_FrameInterval)
		{
			if (m_Playstate == PlayState::FORWARDS)
				PlayNextFrame();
			else if (m_Playstate == PlayState::BACKWARDS)
				PlayPreviousFrame();

			timer.restart();
		}

		/* Sleep to yield cpu */
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void Timeline::TimerPlaybackLoop()
{
	m_Playstate == PlayState::FORWARDS ? PlayNextFrame() : PlayPreviousFrame();
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
	ResetRange();

	/* Update timeslider range */
	m_Timeslider->setRange(min, max);
	m_Timeslider->m_CachedFrames.reserve(max - min + 1);

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

void Timeline::Clear()
{
	m_Timeslider->setRange(0, 1);

	m_Timeslider->ClearAnnotatedFrames();
	m_Timeslider->ClearCachedFrames();

	m_Start = 0;
	m_End = 1;
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
	Stop();

	m_Playstate = PlayState::FORWARDS;
	StartPlayback();
	emit playbackStateChanged(m_Playstate);
}

void Timeline::PlayBackwards()
{
	Stop();

	m_Playstate = PlayState::BACKWARDS;
	StartPlayback();
	emit playbackStateChanged(m_Playstate);
}

void Timeline::Stop()
{
	m_Playing = false;
	m_Playstate = PlayState::STOPPED;

	m_PlayTimer.stop();
	emit playbackStateChanged(m_Playstate);

	// /* Wait for the task to complete */
	// if (m_Worker.valid())
	// 	m_Worker.get();
}

void Timeline::Replay()
{
	if (m_Playstate == PlayState::FORWARDS)
		PlayForwards();
	else if (m_Playstate == PlayState::BACKWARDS)
		PlayBackwards();
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
		switch (m_LoopType)
		{
			case LoopType::PlayOnce:
				Stop();
				break;
			case LoopType::PingPong:
				PlayBackwards();
				break;
			case LoopType::LoopInfinitely:
				emit mediaFinished(PlayState::FORWARDS);
			default:
				m_Timeslider->setValue(m_Start);
		}
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
		switch (m_LoopType)
		{
			case LoopType::PlayOnce:
				Stop();
				break;
			case LoopType::PingPong:
				PlayForwards();
				break;
			case LoopType::LoopInfinitely:
				emit mediaFinished(PlayState::BACKWARDS);
			default:
				m_Timeslider->setValue(m_End);
		}
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
	state == Timeline::PlayState::FORWARDS ? PlayForwards() : PlayBackwards();
}

VOID_NAMESPACE_CLOSE
