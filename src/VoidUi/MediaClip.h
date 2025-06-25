#ifndef _VOID_MEDIA_CLIP_H
#define _VOID_MEDIA_CLIP_H

/* STD */
#include <memory>

/* Qt */
#include <QObject>
#include <QColor>

/* Internal */
#include "Definition.h"
#include "VoidObject.h"
#include "VoidCore/Media.h"

VOID_NAMESPACE_OPEN

/* Forward Declaration for typedef */
class MediaClip;
typedef std::shared_ptr<MediaClip> SharedMediaClip;

class MediaClip : public VoidObject
{
    Q_OBJECT

public:
    MediaClip(QObject* parent = nullptr);
    MediaClip(const Media& media, QObject* parent = nullptr);
    virtual ~MediaClip();

    inline Media GetMedia() const { return m_Media; }

    inline void SetColor(const QColor& color)
    {
        m_Color = color;
        /* A change has been made */
        emit updated();
    }

    inline void SetMedia(const Media& media)
    { 
        /* Update the media */
        m_Media = media;

        /* and emit that something has been updated */
        emit updated();
    }

    inline QColor Color() const { return m_Color; }

    /* Overloading Base Media functions to allow frameCached be emit */
    inline SharedPixels Image(const int frame)
    { 
        /* Emit that the frame was cached */
        emit frameCached(frame);

        /* Return the frame data from media */
        return m_Media.Image(frame);
    }

    /* 
     * Caches all the frames for the media
     * emits frameCached for all the frames as they are cached
     * Overloads the function Cache from media
     */
    void Cache()
    {
        /* For each frame in Media -> Cache the frame and emit the signal that a frame has been cached */
        for (std::pair<const int, Frame>& it: m_Media)
        {
            /* Cache the data for the frame */
            it.second.Cache();

            /* Emit the frame which was cached */
            emit frameCached(it.first);
        }
    }

    /* Exposed Media Function Accessors */
    inline std::string Name() const { return m_Media.Name(); }
    inline std::string Extension() const { return m_Media.Extension(); }
    inline int FirstFrame() const { return m_Media.FirstFrame(); }
    inline int LastFrame() const { return m_Media.LastFrame(); }

    inline int Duration() const { return (LastFrame() - FirstFrame()) + 1; }

    /*
     * Returns whether a given frame falls in the range of Media
     * i.e. between the first and the last frame of media
     * Any frame missing does not matter as this method only returns whether a frame is in the range or not
     */
    [[nodiscard]] inline bool HasFrame(const int frame) const { return m_Media.HasFrame(frame); }

    /* 
     * Returns whether a given frame is available to read
     * There could be a scenario where the given frame is in the range of first - last but is not available
     * and is referred to as the missing frame.
     */
    [[nodiscard]] inline bool Contains(const int frame) const { return m_Media.Contains(frame); }

    /*
     * Based on the available frames, returns the frame which is just lower than the provided frame
     * This is used when the current frame is not available but we want the neartest frame to be used in it's place
     */
    inline int NearestFrame(const int frame) const { return m_Media.NearestFrame(frame); };

    inline Frame FirstFrameData() const { return m_Media.FirstFrameData(); }
    inline Frame LastFrameData() const { return m_Media.LastFrameData(); }

    inline SharedPixels FirstImage() { return Image(FirstFrame()); }
    inline SharedPixels LastImage() { return Image(LastFrame()); }

    inline double Framerate() const { return m_Media.Framerate(); }
    inline bool Empty() const { return m_Media.Empty(); }

    /*
     * Clears the cache for all the frames of the Media
     */
    inline void ClearCache() { m_Media.ClearCache(); }

    /* Stops caching if the cache process is ongoing */
    inline void StopCaching() { m_Media.StopCaching(); }

    /* Returns whether the Cache process is ongoing */
    inline bool Caching() const { return m_Media.Caching(); }

signals: /* Signals defining any change that has happened */
    /*
     * Defines if the media or any entity internally has been updated
     * This is an intimation for other entities relying on this to update themselves
     */
    void updated();

    /*
     * Emitted when a frame is cached
     * The cache could happen when the media cache operation is run continuously on a thread
     * Or if the frame is queried by the viewport
     */
    void frameCached(int frame);

private: /* Members */
    /* The Media it holds for playback */
    Media m_Media;
    QColor m_Color;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_CLIP_H
