// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_TIMESLIDER_H
#define _VOID_TIMESLIDER_H

/* STD */
#include <vector>
#include <unordered_map>

/* Qt */
#include <QSlider>

/* Internal */
#include "QDefinition.h"

VOID_NAMESPACE_OPEN

/* Forward Declaration for Timeline class */
class Timeline;

class Timeslider : public QSlider
{
public:
	Timeslider(Qt::Orientation orientation, QWidget *parent = nullptr);

	virtual ~Timeslider();

	/**
	 * Update the range for playback in the timeslider
	 */
	void SetUserFirstframe(int frame);
	void SetUserEndframe(int frame);

	void ResetStartFrame();
	void ResetEndFrame();
	void ResetRange();

	inline int Minimum() const { return m_UserStartframe ? m_UserStartframe : minimum(); } 
	inline int Maximum() const { return m_UserEndframe ? m_UserEndframe : maximum(); }

	/* Adds a cache marker on the frame */
	void AddCacheFrame(int frame);
	void RemoveCachedFrame(int frame);
	void ClearCachedFrames();

	/* Adds an annotation marker on the frame */
	void AddAnnotatedFrame(int frame);
	void RemoveAnnotatedFrame(int frame);
	void ClearAnnotatedFrames();
	void SetAnnotatedFrames(const std::vector<int>& frames);
	void SetAnnotatedFrames(std::vector<int>&& frame);

	/**
	 * Checks whether a given frame is in Playable range
	 */
	inline bool InRange(int frame) { return frame >= Minimum() && frame <= Maximum(); }

protected:
	void enterEvent(EnterEvent* event) override;
	void leaveEvent(QEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void paintEvent(QPaintEvent* event) override;

private: /* Methods */
	void UpdateHovered(int xpos);

	/* Returns whether the requested frame is annotated? */
	inline bool Annotated(int frame) const
	{ 
		return std::find(m_AnnotatedFrames.begin(), m_AnnotatedFrames.end(), frame) != m_AnnotatedFrames.end(); 
	}

	/* Returns whether the requested frame is cached? */
	inline bool Cached(int frame) const
	{
		return std::find(m_CachedFrames.begin(), m_CachedFrames.end(), frame) != m_CachedFrames.end();
	}

private: /* Members */
	bool m_Focussed;
	int m_HovXPos;
	int m_HoveredFrame;

	/* Stores any frame that have been marked as Cached for the timeslider */
	std::vector<int> m_CachedFrames;
	/* Stores any frame that has been annotated */
	std::vector<int> m_AnnotatedFrames;

	/**
	 * By default the timeline has a minimum and maximum value
	 *
	 * 		|----------------------------|
	 *		100						   200
	 *
	 * And the user can still specify an in-out within the default timeline
	 *
	 *              30          60
	 * 		|-------[------------]-------|
	 *		100						   200
	 *
	 * And when the playback happens, it happens over the user specified range i.e. 30 - 60
	 */

	/* The User set Minimum Frame for playback */
	int m_UserStartframe;
	/* The User set Maximum Frame for playback */
	int m_UserEndframe;

	/* Friends */
	friend class Timeline;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_TIMESLIDER_H
