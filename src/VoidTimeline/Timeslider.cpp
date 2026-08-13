// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QMouseEvent>
#include <QPainter>
#include <QStyle>

/* Internal */
#include "Timeslider.h"
#include "VoidCore/Timekeeper.h"

/* Timeslider Markings Step */
const int SL_MARKING_STEP = 5;

/* Timeslider Fixed height */
const int SL_HEIGHT = 30;

/* Color for the frame displayed on the timeslider */
const QColor SL_FRAME_COLOR = {190, 150, 60};

/* Color for the cache denotion of the timeslider */
const QColor SL_CACHE_COLOR = {190, 150, 60};

/* Color for the annotate frame denotion of the timeslider */
const QColor SL_ANNOTATED_COLOR = {70, 130, 250};

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

	/* Update the hovered information */
	if (m_Focussed)
		UpdateHovered(event->pos().x());

	/* Trigger a repaint */
	update();
}

void Timeslider::mousePressEvent(QMouseEvent* event)
{
	m_Focussed = false;

	/* Set the value on the slider from postion of the mouse press */
	const int value = QStyle::sliderValueFromPosition(
					minimum(),
					maximum(),
					event->pos().x(),
					width(),
					orientation() == Qt::Vertical
				);
	setValue(value);
	Timekeeper::Instance().SetFrame(value);

	// User clicked on the current position calls for a seek in the playable timeline
	emit seeked(value);

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
	QPainter painter(this);

	int startpos = 0, endpos = 0;
	const float uwidth = float(width()) / (maximum() - minimum());
	const float halfuwidth = uwidth * 0.5f;

	// const int hpos = width() * (value() - minimum()) / std::max((maximum() - minimum()), 1);
	const int step = (maximum() - minimum() + 1) / std::max((maximum() - minimum() + 1) / SL_MARKING_STEP, 1);
	const float rec_range = 1.f / (maximum() - minimum() + 1);

	painter.setPen(QColor(30, 30, 30));
	painter.setBrush(QBrush(QColor(30, 30, 30)));

	painter.fillRect(rect(), Qt::black);

	/**
	 * This rect will have a slightly lighter shade to indicate playing range
	 * This range is obviously affected by the user defined in-out set
	 * making the overall playable range more prominent and visible to the user
	 */
	QRect r(rect());

	if (m_UserStartframe)
	{
		startpos = (m_UserStartframe - minimum()) * uwidth;
		r.setLeft(startpos);
	}

	if (m_UserEndframe)
	{
		endpos = (m_UserEndframe - minimum()) * uwidth;
		r.setRight(endpos);
	}

	painter.fillRect(r, palette().color(QPalette::AlternateBase));

	painter.setBrush(palette().color(QPalette::Highlight));
	painter.drawRect(
		// Handle pos - half of the width of the handle
		width() * (value() - minimum()) / std::max((maximum() - minimum()), 1) - std::max(uwidth, 4.0f) * 0.5f,
		0,
		std::max(uwidth, 4.0f),
		height()
	);
	/* }}} */

	for (int i = minimum(); i <= maximum(); i += step)
	{
		painter.setPen(QPen(Qt::gray, 1));
		painter.drawLine(
			width() * (i - minimum()) * rec_range, height() - 10,
			width() * (i - minimum()) * rec_range, height()
		);
	}

	for (int frame : m_CachedFrames)
	{
		painter.setPen(QPen(SL_CACHE_COLOR, 3));
		painter.drawLine((frame - minimum()) * uwidth, 0, (frame - minimum()) * uwidth + uwidth, 0);
	}

	for (int frame : m_AnnotatedFrames)
	{
		painter.setPen(QPen(SL_ANNOTATED_COLOR, 3));
		painter.drawLine((frame - minimum()) * uwidth - halfuwidth, 6, (frame - minimum()) * uwidth + halfuwidth, 6);
	}

	if (startpos)
	{
		painter.setPen(QPen(QColor(200, 75, 60), 2));
		painter.drawLine(startpos, 0, startpos, height());
		painter.drawText(startpos + 4, height() * 0.5f, QString::number(m_UserStartframe));
	}

	if (endpos)
	{
		painter.setPen(QPen(QColor(200, 75, 60), 2));
		painter.drawLine(endpos, 0, endpos, height());
		painter.drawText(endpos + 4, height() * 0.5f, QString::number(m_UserEndframe));
	}

	if (m_Focussed)
	{
		painter.setPen(SL_FRAME_COLOR);
		painter.drawText(m_HovXPos - halfuwidth, height() * 0.5f, QString::number(m_HoveredFrame));
	}
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

void Timeslider::AddCacheFrame(int frame)
{
	m_CachedFrames.push_back(frame);

	/* Repaint after a frame has been cached to redraw the cache line */
	update();
}

void Timeslider::RemoveCachedFrame(int frame)
{
	std::vector<int>::iterator it = std::find(m_CachedFrames.begin(), m_CachedFrames.end(), frame);

	/*
	 * If the value is not present in cached frames already
	 * Add it to the set
	 */
	if (it != m_CachedFrames.end())
	{
		m_CachedFrames.erase(it);
		/* Repaint after a frame has been cached to redraw the cache line */
		update();
	}
}

void Timeslider::ClearCachedFrames()
{
	/* Clears the contents of the cached frames */
	m_CachedFrames.clear();
	m_CachedFrames.resize(0);
	m_CachedFrames.shrink_to_fit();

	/* Repaint after the cache frames have been cleared */
	update();
}

void Timeslider::AddAnnotatedFrame(int frame)
{
	m_AnnotatedFrames.push_back(frame);

	/* Redraw now that we have annotated it */
	update();
}

void Timeslider::RemoveAnnotatedFrame(int frame)
{
	// auto it = std::find(m_AnnotatedFrames.begin(), )
	auto it = std::remove(m_AnnotatedFrames.begin(), m_AnnotatedFrames.end(), frame);
	if (it != m_AnnotatedFrames.end())
	{
		/* Remove the frame from the Annotated frames */
		m_AnnotatedFrames.erase(it, m_AnnotatedFrames.end());
		/* And Redraw */
		update();
	}
}

void Timeslider::SetAnnotatedFrames(const std::vector<int>& frames)
{
	m_AnnotatedFrames = frames;
	/* Redraw */
	update();
}

void Timeslider::SetAnnotatedFrames(std::vector<int>&& frames)
{
	m_AnnotatedFrames = std::move(frames);
	/* Redraw */
	update();
}

void Timeslider::ClearAnnotatedFrames()
{
	/* Clear All frames that were marked annotated */
	m_AnnotatedFrames.clear();

	/* Redraw */
	update();
}

VOID_NAMESPACE_CLOSE
