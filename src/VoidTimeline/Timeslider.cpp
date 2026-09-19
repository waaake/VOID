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
	, m_Timekeeper(Timekeeper::Instance())
	, m_Focussed(false)
	, m_HovXPos(0)
	, m_HoveredFrame(0)
	, m_UserStartframe(0)
	, m_UserEndframe(0)
{
	setFixedHeight(SL_HEIGHT);
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

	m_HoveredFrame = -1;
	m_HovXPos = -1;
	update();
}

void Timeslider::mouseMoveEvent(QMouseEvent* event)
{
	QSlider::mouseMoveEvent(event);
	if (m_Focussed)
		UpdateHovered(event->pos().x());

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

	// Allow dragging behaviour
	QSlider::mousePressEvent(event);
	update();
}

void Timeslider::mouseReleaseEvent(QMouseEvent* event)
{
	if (m_HovXPos != -1)
	{
		m_Focussed = true;
		UpdateHovered(event->pos().x());
	}

	QSlider::mouseReleaseEvent(event);
	update();
}

void Timeslider::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);

	int startpos = 0, endpos = 0;
	const int minval = minimum();
	const int maxval = maximum();

	const int diff = maxval - minval;

	const float uwidth = float(width()) / diff;
	const float halfuwidth = uwidth * 0.5f;

	// const int hpos = width() * (value() - minimum()) / std::max((maximum() - minimum()), 1);
	const int step = std::max((diff + 1) / TickCount(diff + 1), 1);
	const float rec_range = 1.f / (diff + 1);

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
		startpos = (m_UserStartframe - minval) * uwidth;
		r.setLeft(startpos);
	}

	if (m_UserEndframe)
	{
		endpos = (m_UserEndframe - minval) * uwidth;
		r.setRight(endpos);
	}

	painter.fillRect(r, palette().color(QPalette::Dark).lighter(120));
	painter.setBrush(palette().color(QPalette::Highlight));
	painter.drawRect(QRect(
		// Handle pos - half of the width of the handle
		width() * (value() - minval) / std::max((diff), 1) - std::max(uwidth, 4.0f) * 0.5f,
		0,
		std::max(uwidth, 4.0f),
		height()
	));

	for (int i = minval, count = 0; i <= maxval; i += step, ++count)
	{
		painter.setPen(QPen(Qt::gray, 1));
		painter.drawLine(
			width() * (i - minval) * rec_range, height() - ((count % 5) == 0 ? 15 : 10),
			width() * (i - minval) * rec_range, height()
		);
	}

	for (int frame : m_CachedFrames)
	{
		painter.setPen(QPen(SL_CACHE_COLOR, 3));
		painter.drawLine((frame - minval) * uwidth, 0, (frame - minval) * uwidth + uwidth, 0);
	}

	for (int frame : m_AnnotatedFrames)
	{
		painter.setPen(QPen(SL_ANNOTATED_COLOR, 3));
		painter.drawLine((frame - minval) * uwidth - halfuwidth, 6, (frame - minval) * uwidth + halfuwidth, 6);
	}

	if (startpos)
	{
		painter.setPen(QPen(QColor(200, 75, 60), 2));
		painter.drawLine(startpos, 0, startpos, height());
		painter.drawText(
			QRect(0, 0, startpos - 4, height()),
			Qt::AlignRight | Qt::AlignTop,
			m_Timekeeper.DisplayFrame(m_UserStartframe).c_str()
		);
	}

	if (endpos)
	{
		painter.setPen(QPen(QColor(200, 75, 60), 2));
		painter.drawLine(endpos, 0, endpos, height());
		painter.drawText(
			QRect(endpos + 4, 0, rect().width(), height()),
			Qt::AlignLeft | Qt::AlignTop,
			m_Timekeeper.DisplayFrame(m_UserEndframe).c_str()
		);
	}

	if (m_Focussed)
	{
		painter.setPen(SL_FRAME_COLOR);
		r = rect();
		QFlags<Qt::AlignmentFlag> f = Qt::AlignTop;
		if (m_HoveredFrame < minval + (maxval - minval) * 0.2)
		{
			f = Qt::AlignTop | Qt::AlignLeft;
			r.setLeft(m_HovXPos);
		}
		else if (m_HoveredFrame > minval + (maxval - minval) * 0.8)
		{
			f = Qt::AlignTop | Qt::AlignRight;
			r.setRight(m_HovXPos);
		}
		else
		{
			f = Qt::AlignTop | Qt::AlignHCenter;
			r.setLeft(m_HovXPos - 100);
			r.setRight(m_HovXPos + 100);
		}

		painter.drawText(r, f, m_Timekeeper.DisplayFrame(m_HoveredFrame).c_str());
	}
}

void Timeslider::UpdateHovered(int xpos)
{
	// Set the Hovered position based on the x position of the event click
	m_HovXPos = xpos;
	// Fetch the slider value based on the position
	m_HoveredFrame = QStyle::sliderValueFromPosition(
						minimum(),
						maximum(),
						m_HovXPos,
						width(),
						orientation() == Qt::Vertical
					);
}

int Timeslider::TickCount(int duration) const
{
	if (duration < 20) return duration;
	if (duration < 500) return 100;
	if (duration < 10000) return 150;
	return 200;
}

void Timeslider::SetUserFirstframe(int frame)
{
	m_UserStartframe = frame;
	update();
}

void Timeslider::SetUserEndframe(int frame)
{
	m_UserEndframe = frame;
	update();
}

void Timeslider::ResetRange()
{
	m_UserStartframe = 0;
	m_UserEndframe = 0;

	update();
}

void Timeslider::ResetStartFrame()
{
	m_UserStartframe = 0;
	update();
}

void Timeslider::ResetEndFrame()
{
	m_UserEndframe = 0;
	update();
}

void Timeslider::AddCacheFrame(int frame)
{
	m_CachedFrames.push_back(frame);
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
	update();
}

void Timeslider::RemoveAnnotatedFrame(int frame)
{
	auto it = std::remove(m_AnnotatedFrames.begin(), m_AnnotatedFrames.end(), frame);
	if (it != m_AnnotatedFrames.end())
	{
		m_AnnotatedFrames.erase(it, m_AnnotatedFrames.end());
		update();
	}
}

void Timeslider::SetAnnotatedFrames(const std::vector<int>& frames)
{
	m_AnnotatedFrames = frames;
	update();
}

void Timeslider::SetAnnotatedFrames(std::vector<int>&& frames)
{
	m_AnnotatedFrames = std::move(frames);
	update();
}

void Timeslider::ClearAnnotatedFrames()
{
	m_AnnotatedFrames.clear();
	update();
}

VOID_NAMESPACE_CLOSE
