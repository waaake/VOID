/* Qt */
#include <QMouseEvent>
#include <QPainter>
#include <QStyle>

/* Internal */
#include "Timeslider.h"

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

/* Color for the annotate frame denotion of the timeslider */
static const QColor SL_ANNOTATED_COLOR = {70, 130, 250};

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
	double halfuwidth = uwidth / 2;

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

	int range = maximum() - minimum();

	/* Number of frames to be drawn */
	int number = std::max(range / SL_MARKING_STEP, 1);	// ensure that we don't divide by zero

	/* Step here would give the step based on the number of markings are being generated */
	int step = range / number;

	/**
	 * One Big Loop over all the frames
	 * Checks if the frame is Cached? -> Color Cache
	 * Checks if the frame is Annotated? -> Color Annotated
	 * Is that a frame on which we're supposed to Draw a Line? -> Draw a Line
	 */
	for (int i = minimum(); i <= maximum(); i++)
	{
		/* Position of the frame */
		int xpos = (i - minimum()) * uwidth;

		/* Cache {{{ */
		if (Cached(i))
		{
			painter.setPen(QPen(SL_CACHE_COLOR, 3));
			/* Draw line representing the frame which has been cached */
			painter.drawLine(xpos, 0, xpos + uwidth, 0);
		}
		/* }}}*/

		/* Annotation {{{ */
		if (Annotated(i))
		{
			painter.setPen(QPen(SL_ANNOTATED_COLOR, 3));
			/* Draw line representing that the frame has been cached */
			painter.drawLine(xpos - halfuwidth, 6, xpos + halfuwidth, 6);
		}
		/* }}}*/
		
		/* Line {{{ */
		if (step && !(i % step))
		{
			/* Update pen to draw Pre-marked frames on the timeslider */
			painter.setPen(QPen(Qt::gray, 2));
			/* Position of the line */
			int pos = width() * (i - minimum()) / range;
			/* Draw Line representing Marked frames on the timeslider */
			painter.drawLine(pos, height() - 10, pos, height());
		}
		/* }}}*/
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

void Timeslider::AddAnnotatedFrame(int frame)
{
	auto it = std::find(m_AnnotatedFrames.begin(), m_AnnotatedFrames.end(), frame);

	/**
	 * Frame doesn't exist already
	 */
	if (it == m_AnnotatedFrames.end())
	{
		/* Add to the annotated frames */
		m_AnnotatedFrames.push_back(frame);

		/* Redraw now that we have annotated it */
		update();
	}
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

void Timeslider::ClearAnnotatedFrames()
{
	/* Clear All frames that were marked annotated */
	m_AnnotatedFrames.clear();

	/* Redraw */
	update();
}

VOID_NAMESPACE_CLOSE
