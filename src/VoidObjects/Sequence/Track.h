// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_TRACK_H
#define _VOID_TRACK_H

/* STD */
#include <memory>
#include <map>
#include <vector>

/* Qt */
#include <QObject>
#include <QColor>

/* Internal */
#include "Definition.h"
#include "TrackItem.h"
#include "VoidObjects/VoidObject.h"

VOID_NAMESPACE_OPEN

/* Forward Declaration of the Sequence which holds the Track */
class PlaybackSequence;
class PlaybackTrack;

/*
 * Let the Tracks be auto managed by reference count whether they survive
 * in the world or die or even get killed
 */
typedef std::shared_ptr<PlaybackTrack> SharedPlaybackTrack;

class VOID_API TrackMap
{
public:
    /**
     * Adds track item to the mapping.
     */
    void Add(const SharedTrackItem& item);

    /**
     * Removes the track item from the mapping.
     */
    void Remove(const SharedTrackItem& item);

    void Clear();

    /**
     * Returns a Track Item present at a given frame, if it exists
     * else a null pointer is returned.
     */
    SharedTrackItem At(const int frame) const;

    inline bool Empty() const { return m_Frames.empty(); }

private: /* Members */
    /**
     * This mapping holds the first frame of Track Item mapped to the Shared track item
     * The intent is to be able to get a lower frame from any provided frame to get an item which may be present
     * at the given frame
     *
     * e.g. If we have a track holding items like
     *  __________  ________        _______  ___________
     * |__________||________|------|_______||___________|
     * 1        20 21     30       50    60  61        80
     *
     * and our internal map would look like 
     * {
     *      1  -> TrackItem1
     *      21 -> TrackItem2
     *      50 -> TrackItem3
     *      61 -> TrackItem4
     * }
     * 
     * Then if we were to pass in frame 10, this logic is going to find the nearest lower bound frame to 10
     * which will result in 1 and we take a look at the track item at frame 1 and check if frame 10 (with the offset ofcourse)
     * belongs to it, if yes we return the track item
     *
     * For another case, if some one is to pass a frame 35, the avaialable lower bound frame in the vector or map is 21
     * we get the item at frame 21 and check if frame 35 (with offset) is in it's range, it won't be so we return nullptr back
     * indicating that nothing exists at that point.
     */
    std::map<int, SharedTrackItem> m_Items;
    std::vector<int> m_Frames;

    /**
     * Inner iterator to expose the TrackItems' iterator directly from this class
     * this allows having only one Structure being used at the Track level to hold track items
     */
    class Iterator
    {
        /* Members */
        using Iter = std::map<int, SharedTrackItem>::iterator;

        /* Internal iterator */
        Iter it;

    public:
        explicit Iterator(Iter it)
            : it(it) {}

        /* Dereference operator */
        SharedTrackItem& operator*() { return it->second; }
        /**
         * Increment operator
         * Increment internal iterator and return the instance reference
         */
        Iterator& operator++() { ++it; return *this; }

        /* Not equals */
        bool operator!=(const Iterator& other) { return it != other.it; }
    };

public: /* Iterator */
    Iterator begin() { return Iterator(m_Items.begin()); }
    Iterator end() { return Iterator(m_Items.end()); }
};

class VOID_API PlaybackTrack : public VoidObject
{
    Q_OBJECT

public:
    PlaybackTrack(QObject* parent = nullptr);

    virtual ~PlaybackTrack();

    /*
     * Clears anything in the track and sets the provided media as first
     */
    void SetMedia(const SharedMediaClip& media);

    /* Set a color for the Track */
    inline void SetColor(const QColor& color)
    {
        /* Update the color for the track */
        m_Color = color;
        /* Emit a changed signal */
        emit updated();
    }

    /*
     * Appends the Media to the already existing track of Medis files
     * Which will get played in order
     */
    void AddMedia(const SharedMediaClip& media);

    /* Clears the Playback Track */
    void Clear();

    /*
     * Caches all frames of any media on the track
     * emits frameCached for every frame that has been cached
     */
    void Cache();

    /* Clears all cache from internal media and emits cacheCleared */
    void ClearCache();

    /* Getters */
    inline int StartFrame() const { return m_StartFrame; }
    inline int EndFrame() const { return m_EndFrame; }

    inline bool IsEmpty() const { return m_Items.Empty(); }

    /* Returns the Color associated with the Track */
    inline QColor Color() const { return m_Color; }

    /**
     * Describes whether a track is active for playback or taking in elements with menu options
     */
    [[nodiscard]] inline bool Active() const { return m_Visible && m_Enabled; }

    inline bool Visible() const { return m_Visible; }
    inline bool Enabled() const { return m_Enabled; }

    /**
     * From the track, return the track item which is present at a given frame in the timeline
     * Returns nullptr if there is no trackitem at the given timeframe
     */
    inline SharedTrackItem GetTrackItem(const int frame) const { return m_Items.At(frame); }

    /* The parent of the Track should always be a Sequence, in case it exists inside a Sequence */
    inline PlaybackSequence* Sequence() const { return reinterpret_cast<PlaybackSequence*>(parent()); }

    /* Setters */

    /**
     * The track's range is always defined by the track items in it
     * The only thing which can/should be changed of a track is the starting frame
     */
    inline void SetStartFrame(int start) { SetRange(start, start + m_EndFrame); }

signals: /* Signals Denoting actions in the Track */
    void mediaAdded();
    void cleared();

    /* This signal denotes that something in the track was changed/modified i.e. updated */
    void updated();

    /**
     * Emitted when the time range of the track has changed
     * includes the start and end frame of the track
     */
    void rangeChanged(int start, int end);

    /**
     * Emitted when a frame is cached
     * The cache could happen when the media cache operation is run continuously on a thread
     * Or if the frame is queried by the viewport
     */
    void frameCached(int frame);

    /* Emitted when the cache of all items' media has been cleared internally */
    void cacheCleared();
protected: /* Members */
    /* Holds the Media which have been added to the Track */
    TrackMap m_Items;

    int m_StartFrame, m_EndFrame;
    int m_Duration;

    /* State whether the track is visible */
    bool m_Visible;
    /* State whether the track is enabled */
    bool m_Enabled;

    /* The color associated with the track */
    QColor m_Color;

protected: /* Methods */
    void SetRange(int start, int end, const bool inclusive = true);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_TRACK_H
