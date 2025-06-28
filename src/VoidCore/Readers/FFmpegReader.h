#ifndef _VOID_FFMPEG_READER_H
#define _VOID_FFMPEG_READER_H

/* STD */
#include <vector>
#include <unordered_map>

/* FFMpeg */
extern "C" 
{
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}

/* Internal */
#include "Definition.h"
#include "PixReader.h"

VOID_NAMESPACE_OPEN

class FFmpegDecoder
{
public:
    static FFmpegDecoder& Instance()
    {
        static FFmpegDecoder instance;
        return instance;
    }

    FFmpegDecoder();

    FFmpegDecoder(const FFmpegDecoder&) = delete;
    FFmpegDecoder operator=(const FFmpegDecoder&) = delete;

    /**
     * Opens the movie file and decodes the frame
     * keeps the packet iterator in its state and does not destroy the context till a new filepath is
     * provided which means that subsequent queries to get consecutive/immediate frames would just be of
     * complexity O(1) generally as we're not iterating over the queried frame again
     * but sometimes the containers have frames not in order and that can lead to a next frame complexity of
     * O(n) as the iterator might have to loop over all frames to get to the one we're looking at which also
     * caches the other frames so next frame queries could directly result in direct data transfer
     */
    void Decode(const std::string& path, const int framenumber);

    inline std::vector<unsigned char>& GetData(const int framenumber) { return GetVector(framenumber); }

    [[nodiscard]] int Width() const { return m_Width; }
    [[nodiscard]] int Height() const { return m_Height; }
    [[nodiscard]] int Channels() const { return m_Channels; }

private: /* Members */
    std::string m_Path;

    int64_t m_CurrentFrame;

    int m_Width, m_Height, m_Channels;

    std::vector<unsigned char> m_Pixels;

    /* FFMPEG Contexts */
    AVFormatContext* m_FormatContext;
    AVCodecContext* m_CodecContext;
    AVFrame* m_Frame;
    AVFrame* m_RGBFrame;
    AVPacket* m_Packet;
    SwsContext* m_SwsContext;
    AVStream* m_Stream;

    int m_StreamID;

    /**
     * The Map to save Data for each of the frame
     */
    std::unordered_map<int, std::vector<unsigned char>> m_DecodedFrames;

private: /* Methods */
    void Open();
    void Close();

    std::vector<unsigned char>& GetVector(const int frame);
};


class FFmpegPixReader : public VoidMPixReader
{
public:
    FFmpegPixReader();
    FFmpegPixReader(const std::string& path);

    virtual ~FFmpegPixReader();

    /**
     * Reads the provided image file's data into underlying structs
     */
    virtual void Read(const std::string& path, int framenumer) override;

    /**
     * Returns the OpenGL data type
     * e.g. GL_UNSIGNED_BYTE, GL_FLOAT
     */
    inline virtual unsigned int GLType() const override { return VOID_GL_UNSIGNED_BYTE; }

    /**
     * Returns OpenGL channel format
     * GL_RGBA | GL_RGB
     */
    inline virtual unsigned int GLFormat() const override { return (m_Channels == 3) ? VOID_GL_RGB : VOID_GL_RGBA; }

    /**
     * Returns the Pointer to the underlying pixel data which will be rendered on the Renderer
     * This allows the deriving class full control over the data type, as long as the data
     * is correct to be rendered on GL Viewer, this can be returned from here
     */
    inline virtual const void* Pixels() const override { return m_Pixels.data(); }

    /**
     * Returns the frame data as unsigned char*
     * This would be used to create thumbnails for qt
     * Not all frames will be used so this function can create a vector on the fly if unsigned char
     * is not the base datatype of the class
     */
    inline virtual const unsigned char* ThumbnailPixels() const override { return m_Pixels.data(); }

    /**
     * Image Specifications
     * Dimensions and Channel information for the Image
     */
    inline virtual int Width() const override { return m_Width; }
    inline virtual int Height() const override { return m_Height; }
    inline virtual int Channels() const override { return m_Channels; }

    /**
     * Clear internal pixel data
     * This is here to allow memory to be freed when needed
     */
    virtual void Clear() override;

    /**
     * Returns if the underlying struct has any pixel data
     */
    inline virtual bool Empty() override { return m_Pixels.empty(); }

    /**
     * Updates the internal filepath to process needed information
     */
    inline virtual void UpdatePath(const std::string& path) override { m_Path = path; };

    /**
     * Returns the frame range information of the Movie media
     */
    virtual MFrameRange Framerange() override;

    /**
     * Returns the framerate of the movie media
     */
    virtual double Framerate() override;

private: /* Members */
    std::string m_Path;

    /* Image specifications */
    int m_Width, m_Height;
    /* Number of channels in the image */
    int m_Channels;

    /**
     * Media timeline information
     */
    v_frame_t m_Startframe;
    v_frame_t m_Endframe;
    v_frame_t m_Duration;
    double m_Framerate;

    /* Internal data store */
    std::vector<unsigned char> m_Pixels;

private: /* Methods */
    /**
     * Processes the basic media information like framerate,
     * start and end frames and anything additional that could describe the media
     */
    void ProcessInformation();

};

VOID_NAMESPACE_CLOSE

#endif // _VOID_FFMPEG_READER_H
