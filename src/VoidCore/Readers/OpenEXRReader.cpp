// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifdef _WIN32
/* https://github.com/AcademySoftwareFoundation/Imath/issues/212
 * This needs to be declared else we get unresolved symbol from imath library
 * unresolved external symbol imath_half_to_float_table
 * The declaration of imath_half_to_float_table picks up the extern from the extern "C"
 */
#define IMATH_HALF_NO_LOOKUP_TABLE
#endif

/* STD */
#include <algorithm>
#include <map>

/* OpenEXR */
#include <OpenEXR/ImfArray.h>
#include <OpenEXR/ImfChannelList.h>
#include <OpenEXR/ImfCompression.h>
#include <OpenEXR/ImfFloatAttribute.h>
#include <OpenEXR/ImfHeader.h>
#include <OpenEXR/ImfPixelType.h>
#include <OpenEXR/ImfInputFile.h>
#include <OpenEXR/ImfIntAttribute.h>
#include <OpenEXR/ImfRgbaFile.h>
#include <OpenEXR/ImfStringAttribute.h>
#include <OpenEXR/ImfChromaticitiesAttribute.h>

#include <OpenEXR/ImfMultiPartInputFile.h>
#include <OpenEXR/ImfFrameBuffer.h>
#include <OpenEXR/ImfInputPart.h>

/* TBB */
#include <tbb/parallel_for.h>

/* Internal */
#include "OpenEXRReader.h"
#include "VoidCore/Logging.h"
#include "VoidCore/VoidTools.h"
#include "VoidCore/Profiler.h"

VOID_NAMESPACE_OPEN

namespace {

std::map<std::string, std::vector<std::string>> HeaderChannels(const Imf::Header& header)
{
    std::map<std::string, std::vector<std::string>> names;
    const std::string part = header.hasName() ? header.name() + "." : "";
    const Imf::ChannelList& channels = header.channels();

    const static std::set<std::string> rgba({"r", "g", "b", "a", "red", "green", "blue", "alpha"});
    const static std::set<std::string> xyz({"x", "y", "z", "w"});

    for (Imf::ChannelList::ConstIterator it = channels.begin(); it != channels.end(); ++it)
    {
        const std::string channel = it.name();
        const std::size_t pos = channel.rfind(".");

        if (pos != std::string::npos && pos)
        {
            std::string layer = std::string(channel, 0, pos);
            names[part + layer].push_back(channel);
        }
        else
        {
            const std::string channelLower = Tools::to_lower(channel);
            if (rgba.contains(channelLower))
                names[part + "RGBA"].push_back(channel);
            else if (xyz.contains(channelLower))
                names[part + "XYZ"].push_back(channel);
            else
                names[part + "Other"].push_back(channel);
        }
    }

    if (header.hasName() && names.size() == 1)
    {
        std::vector<std::string> chans = names.begin()->second;
        names.clear();
        names[header.name()] = chans;
    }

    return names;
}

// channels is copied, need a better way around this
std::vector<std::string> HeaderStreams(const Imf::Header& header, std::map<std::string, std::vector<std::string>> channels)
{
    std::vector<std::string> streams;
    streams.reserve(channels.size());

    if (channels.contains("RGBA"))
    {
        streams.emplace_back("RGBA");
    }
    else
    {
        for (auto it = channels.begin(); it != channels.end(); ++it)
        {
            const std::string layer = Tools::to_lower(it->first);
            if (layer.find(".rgb") != std::string::npos || layer.ends_with(".combined") || layer == "combined")
            {
                streams.emplace_back(it->first);
                channels.erase(it);
                break;
            }
        }
    }

    if (channels.contains("XYZ"))
        streams.emplace_back("XYZ");

    for (const auto& chan : channels)
    {
        if (chan.first == "RGBA" || chan.first == "XYZ")
            continue;

        streams.push_back(chan.first);
    }

    return streams;
}

void SortChannels(std::vector<std::string>& channels)
{
    // try and work out if channels are *.x, *.y etc or just 'x' 'y' 'z' etc.
    // we want to be careful that we don't match a channel called 'xyz.red' though!
    int score = 0;
    static const std::vector<std::string> xyzTokens({".x", ".y", ".z", ".u", ".v"});
    static const std::vector<std::string> xyzTokens0({"x", "y", "z", "u", "v"});
    for (const auto &chan : channels)
    {
        const std::string lower = Tools::to_lower(chan);
        for (const auto &token : xyzTokens)
        {
            if (lower.find(token) != std::string::npos)
                score++;
        }
        for (const auto &token : xyzTokens0)
        {
            if (lower == token)
                score++;
        }
    }
    const bool xyzuv = score >= 2;

    // now we need to sort the channels so that RGBA gets mapped into 0,1,2,3
    // so we can use reverse alphabetical sorting.
    // OR channels named like *.x, *.y, *.z will also get mapped to 0,1,2
    std::sort(
        channels.begin(),
        channels.end(),
        [&xyzuv](const std::string& a, const std::string& b) {
            if (xyzuv) {
                return Tools::to_lower(a) < Tools::to_lower(b);
            } else {
                return Tools::to_lower(a) > Tools::to_lower(b);
            }
        });

    // The image buffer and shader support at most four interleaved channels. Keep
    // malformed or unconventional layers from overrunning the fixed pixel type array.
    if (channels.size() > 4) {
        channels.resize(4);
    }
}

/// Read a scanline part directly into an interleaved float buffer
///
/// Output layout:
///   pixel 0: channel 0, channel 1, ...
///   pixel 1: channel 0, channel 1, ...
///
/// All channels must have full-resolution sampling (1,1)
bool ReadInterleaved(Imf::InputPart& part, const std::vector<std::string>& channelNames, FloatImage& image)
{
    const Imf::Header& header = part.header();
    const Imath::Box2i dataWindow = header.dataWindow();

    image->width = int64_t(dataWindow.max.x) - dataWindow.min.x + 1;
    image->height = int64_t(dataWindow.max.y) - dataWindow.min.y + 1;

    const size_t channels = channelNames.size();

    image->channels = static_cast<int>(channels);
    image->format = image->channels == 3 ? VOID_GL_RGB : VOID_GL_RGBA;

    if (image->width > std::numeric_limits<int>::max() / image->height)
    {
        VOID_LOG_ERROR("EXR image dimensions overflow");
        return false;
    }

    const std::size_t pixels = static_cast<std::size_t>(image->width) * image->height;
    if (pixels > std::numeric_limits<std::size_t>::max() / channels)
    {
        VOID_LOG_ERROR("EXR image buffer size overflow");
        return false;
    }

    const std::size_t values = pixels * channels;
    image->buffer._buf.assign(values, 0.0f);

    const std::size_t pixelStride = channels * sizeof(float);
    const std::size_t rowStride = image->width * pixelStride;

    Imf::FrameBuffer frameBuffer;

    const Imf::ChannelList& channelList = header.channels();

    for (size_t c = 0; c < channels; ++c)
    {
        const std::string& name = channelNames[c];
        const Imf::Channel* channel = channelList.findChannel(name.c_str());

        if (!channel)
        {
            VOID_LOG_ERROR("Missing Channel: {}", name);
            return false;
        }

        // Direct interleaving is straightforward for full-resolution
        // channels. Subsampled channels need a different memory layout
        if (channel->xSampling != 1 || channel->ySampling != 1)
        {
            VOID_LOG_ERROR("Subsampled EXR channel {}", name);
            return false;
        }

        float* channelPtr = image->buffer._buf.data() + c;
        frameBuffer.insert(name, Imf::Slice::Make(Imf::FLOAT, channelPtr, dataWindow, pixelStride, rowStride, 1, 1, 0.0));
    }

    part.setFrameBuffer(frameBuffer);

    // One full-frame request. OpenEXR handles the underlying
    // compression blocks and decompression scheduling
    part.readPixels(dataWindow.min.y, dataWindow.max.y);

    return true;
}

} // namespace

OpenEXRReader::OpenEXRReader(const std::string& path, v_frame_t framenumber)
    : VoidPixReader(path, framenumber)
{
    Imf::setGlobalThreadCount(16);
}

OpenEXRReader::~OpenEXRReader()
{
    Clear();
}

void OpenEXRReader::Clear()
{
    m_Image->Clear();
}

void OpenEXRReader::ReadThumbnail(const std::string& path, v_frame_t frame, UInt8Image& image)
{
    Imf::RgbaInputFile f(path.c_str(), Tools::thread_count());
    // Image Specs
    Imath::Box2i dw = f.dataWindow();
    image->width = (dw.max.x - dw.min.x) + 1;
    image->height = (dw.max.y - dw.min.y) + 1;

    /* To Get the channels -> Read through the header */
    // const Imf::Header& header = f.header();
    // const Imf::ChannelList& channels = header.channels();

    // /*
    //  * TODO: Check if we have a better way of reading channels rather than iterating here ?
    //  * Update the channel count by reading the channels
    //  */
    // for (Imf::ChannelList::ConstIterator it = channels.begin(); it != channels.end(); ++ it)
    //     m_Channels++;

    // Force 4 Channels
    image->channels = 4;

    // Image Specs
    m_Image->width = image->width;
    m_Image->height = image->height;
    m_Image->channels = image->channels;

    // VOID_LOG_INFO("EXRImage ( Width: {0}, Height: {1}, Channels: {2} )", image->width, image->height, image->channels);

    Imf::Array2D<Imf::Rgba> pixels(image->height, image->width);

    // Read the Pixel data onto the buffer
    f.setFrameBuffer(&pixels[0][0] - dw.min.x - dw.min.y * image->width, 1, image->width);
    f.readPixels(dw.min.y, dw.max.y);

    image->buffer.Resize(image->width * image->height * image->channels);
    const int64_t pixelcount = static_cast<int64_t>(m_Image->width) * m_Image->height;
    const Imf::Rgba* source = &pixels[0][0];
    unsigned char* im = image->buffer.Data();

    tbb::parallel_for(tbb::blocked_range<int64_t>(0, pixelcount), [&](const tbb::blocked_range<int64_t>& r) -> void
    {
        for (int64_t i = r.begin(); i != r.end(); ++i)
        {
            const Imf::Rgba pixel = source[i];
            const int index = i * 4;

            im[index + 0] = static_cast<unsigned char>(std::clamp((float)pixel.r, 0.f, 1.f) * 255.f);
            im[index + 1] = static_cast<unsigned char>(std::clamp((float)pixel.g, 0.f, 1.f) * 255.f);
            im[index + 2] = static_cast<unsigned char>(std::clamp((float)pixel.b, 0.f, 1.f) * 255.f);
            im[index + 3] = static_cast<unsigned char>(std::clamp((float)pixel.a, 0.f, 1.f) * 255.f);
        }
    });
}

void OpenEXRReader::Read(const std::string& path, v_frame_t frame, FloatImage& image)
{
    Imf::MultiPartInputFile input(path.c_str());

    int index = -1;
    std::vector<std::string> channelsToLoad;

    for (int i = 0; i < input.parts(); ++i)
    {
        const Imf::Header& header = input.header(i);

        const std::map<std::string, std::vector<std::string>> channels = HeaderChannels(header);
        const std::vector<std::string> streams = HeaderStreams(header, channels);

        for (const auto& stream : streams)
        {
            if (stream == "RGBA")
            {
                channelsToLoad.clear();
                channelsToLoad = channels.at(stream);

                if (channelsToLoad.empty())
                    break;

                SortChannels(channelsToLoad);

                index = i;
                break;
            }
        }

        if (index >= 0)
            break;
    }

    if (index < 0)
    {
        VOID_LOG_ERROR("No RGBA stream found in {}", m_Path);
        return;
    }

    if (channelsToLoad.empty() || channelsToLoad.size() > 4)
    {
        VOID_LOG_ERROR("Invalid channel count");
        return;
    }

    image->type = VOID_GL_FLOAT;
    Imf::InputPart part(input, index);

    // Read Success
    if (ReadInterleaved(part, channelsToLoad, image))
        return;

    /// There maybe cases, this is partially read and we don't want to display partial information on the viewer at the moment
    /// Better is that we clear the image buffer in that case
    m_Image->Clear();
}

void OpenEXRReader::Read()
{
    Imf::MultiPartInputFile input(m_Path.c_str());

    int index = -1;
    std::vector<std::string> channelsToLoad;

    for (int i = 0; i < input.parts(); ++i)
    {
        const Imf::Header& header = input.header(i);

        const std::map<std::string, std::vector<std::string>> channels = HeaderChannels(header);
        const std::vector<std::string> streams = HeaderStreams(header, channels);

        for (const auto& stream : streams)
        {
            if (stream == "RGBA")
            {
                channelsToLoad.clear();
                channelsToLoad = channels.at(stream);

                if (channelsToLoad.empty())
                    break;

                SortChannels(channelsToLoad);

                index = i;
                break;
            }
        }

        if (index >= 0)
            break;
    }

    if (index < 0)
    {
        VOID_LOG_ERROR("No RGBA stream found in {}", m_Path);
        return;
    }

    if (channelsToLoad.empty() || channelsToLoad.size() > 4)
    {
        VOID_LOG_ERROR("Invalid channel count");
        return;
    }

    m_Image->type = VOID_GL_FLOAT;
    Imf::InputPart part(input, index);

    // Read Success
    if (ReadInterleaved(part, channelsToLoad, m_Image))
        return;

    /// There maybe cases, this is partially read and we don't want to display partial information on the viewer at the moment
    /// Better is that we clear the image buffer in that case
    m_Image->Clear();
}

const std::map<std::string, std::string> OpenEXRReader::Metadata() const
{
    std::map<std::string, std::string> m;

    Imf::InputFile f(m_Path.c_str());
    const Imf::Header& header = f.header();

    /* Basic Metadata */
    m["filepath"] = m_Path;

    int channelCount = 0;
    const Imf::ChannelList& channels = header.channels();

    Imf::PixelType pixelType = Imf::PixelType::HALF;
    for (Imf::ChannelList::ConstIterator it = channels.begin(); it != channels.end(); ++it)
    {
        channelCount++;
        pixelType = it.channel().type;
    }

    /**
     * UINT  = 0, // unsigned int (32 bit)
     * HALF  = 1, // half (16 bit floating point)
     * FLOAT = 2, // float (32 bit floating point)
     */
    if (pixelType == Imf::PixelType::HALF)
        m["pixelType"] = "16 bit floating point";
    else if (pixelType == Imf::PixelType::FLOAT)
        m["pixelType"] = "32 bit floating point";
    else
        m["pixelType"] = "32 bit";

    Imath::Box2i dw = header.dataWindow();
    int width = dw.max.x - dw.min.x + 1;
    int height = dw.max.y - dw.min.y + 1;

    m["width"] = std::to_string(width);
    m["height"] = std::to_string(height);
    m["channels"] = std::to_string(channelCount);

    // const Imf::Compression& compression = header.compression();
    // std::string compressionType;
    // Imf::getCompressionNameFromId(compression, compressionType);
    // m["compression"] = compressionType;

    /* Specific metadata */
    for (Imf::Header::ConstIterator it = header.begin(); it != header.end(); ++it)
    {
        const Imf::Attribute& attr = it.attribute();

        if (std::strcmp(attr.typeName(), "string") == 0)
        {
            const Imf::StringAttribute& sattr = static_cast<const Imf::StringAttribute&>(attr);
            m[it.name()] = sattr.value();
        }
        else if (std::strcmp(attr.typeName(), "float") == 0)
        {
            const Imf::FloatAttribute& fattr = static_cast<const Imf::FloatAttribute&>(attr);
            m[it.name()] = std::to_string(fattr.value());
        }
        else if (std::strcmp(attr.typeName(), "int") == 0)
        {
            const Imf::IntAttribute& iattr = static_cast<const Imf::IntAttribute&>(attr);
            m[it.name()] = std::to_string(iattr.value());
        }
    }

    return m;
}

MediaColor OpenEXRReader::MediaColorInfo() const
{
    MediaColor color;
    color.colorspace = Colorspace::LinearRec709; // Default
    /// Need some more insight on reading exact colorspaces from EXR versions lesser than 3.4

    Imf::InputFile f(m_Path.c_str());
    const Imf::Header& header = f.header();

    const Imf::IntAttribute* acesf = header.findTypedAttribute<Imf::IntAttribute>("acesImageContainerFlag");
    if (acesf)
    {
        if (acesf->value() == 1)
        {
            color.colorspace = Colorspace::ACES2065_1;
            color.transfer = TransferFunction::Linear;

            color.name = "ACES2065-1";
        }

        color.colorspaceMetadata = true;
    }

    // TODO: check chromaticities for a decent match to figure out colorspace
    // const Imf::ChromaticitiesAttribute* chroma = header.findTypedAttribute<Imf::ChromaticitiesAttribute>("chromatacities");

    return color;
}

bool OpenEXRMediaReader::Read(const std::string& path, v_frame_t framenumber, UInt8Image& image)
{
    Imf::RgbaInputFile f(path.c_str(), Tools::thread_count());

    // Image Specs
    Imath::Box2i dw = f.dataWindow();
    image->width = (dw.max.x - dw.min.x) + 1;
    image->height = (dw.max.y - dw.min.y) + 1;

    /* To Get the channels -> Read through the header */
    const Imf::Header& header = f.header();
    const Imf::ChannelList& channels = header.channels();

    m_ColorInfo.colorspace = Colorspace::LinearRec709; // Default
    /// Need some more insight on reading exact colorspaces from EXR versions lesser than 3.4
    const Imf::IntAttribute* acesf = header.findTypedAttribute<Imf::IntAttribute>("acesImageContainerFlag");
    if (acesf)
    {
        if (acesf->value() == 1)
        {
            m_ColorInfo.colorspace = Colorspace::ACES2065_1;
            m_ColorInfo.transfer = TransferFunction::Linear;

            m_ColorInfo.name = "ACES2065-1";
        }

        m_ColorInfo.colorspaceMetadata = true;
    }

    // /*
    //  * TODO: Check if we have a better way of reading channels rather than iterating here ?
    //  * Update the channel count by reading the channels
    //  */
    // for (Imf::ChannelList::ConstIterator it = channels.begin(); it != channels.end(); ++ it)
    //     m_Channels++;

    // Force 4 Channels
    image->channels = 4;

    // Image Specs
    m_Width = image->width;
    m_Height = image->height;
    m_Channels = image->channels;

    // VOID_LOG_INFO("EXRImage ( Width: {0}, Height: {1}, Channels: {2} )", image->width, image->height, image->channels);

    Imf::Array2D<Imf::Rgba> pixels(image->height, image->width);

    // Read the Pixel data onto the buffer
    f.setFrameBuffer(&pixels[0][0] - dw.min.x - dw.min.y * image->width, 1, image->width);
    f.readPixels(dw.min.y, dw.max.y);

    image->buffer.Resize(image->width * image->height * image->channels);
    const int64_t pixelcount = static_cast<int64_t>(image->width) * image->height;
    const Imf::Rgba* source = &pixels[0][0];
    unsigned char* im = image->buffer.Data();

    tbb::parallel_for(tbb::blocked_range<int64_t>(0, pixelcount), [&](const tbb::blocked_range<int64_t>& r) -> void
    {
        for (int64_t i = r.begin(); i != r.end(); ++i)
        {
            const Imf::Rgba pixel = source[i];
            const int index = i * 4;

            im[index + 0] = static_cast<unsigned char>(std::clamp((float)pixel.r, 0.f, 1.f) * 255.f);
            im[index + 1] = static_cast<unsigned char>(std::clamp((float)pixel.g, 0.f, 1.f) * 255.f);
            im[index + 2] = static_cast<unsigned char>(std::clamp((float)pixel.b, 0.f, 1.f) * 255.f);
            im[index + 3] = static_cast<unsigned char>(std::clamp((float)pixel.a, 0.f, 1.f) * 255.f);
        }
    });

    return true;
}

bool OpenEXRMediaReader::Read(const std::string& path, v_frame_t framenumber, FloatImage& image)
{
    Imf::MultiPartInputFile input(path.c_str());

    int index = -1;
    std::vector<std::string> channelsToLoad;

    for (int i = 0; i < input.parts(); ++i)
    {
        const Imf::Header& header = input.header(i);

        const std::map<std::string, std::vector<std::string>> channels = HeaderChannels(header);
        const std::vector<std::string> streams = HeaderStreams(header, channels);

        for (const auto& stream : streams)
        {
            if (stream == "RGBA")
            {
                channelsToLoad.clear();
                channelsToLoad = channels.at(stream);

                if (channelsToLoad.empty())
                    break;

                SortChannels(channelsToLoad);

                index = i;
                break;
            }
        }

        if (index >= 0)
            break;
    }

    if (index < 0)
    {
        VOID_LOG_ERROR("No RGBA stream found in {}", m_Path);
        return false;
    }

    if (channelsToLoad.empty() || channelsToLoad.size() > 4)
    {
        VOID_LOG_ERROR("Invalid channel count");
        return false;
    }

    image->type = VOID_GL_FLOAT;
    Imf::InputPart part(input, index);

    // Read Success
    if (ReadInterleaved(part, channelsToLoad, image))
        return true;

    /// There maybe cases, this is partially read and we don't want to display partial information on the viewer at the moment
    /// Better is that we clear the image buffer in that case
    image->Clear();
    return false;
}

std::map<std::string, std::string> OpenEXRMediaReader::Metadata() const
{
std::map<std::string, std::string> m;

    Imf::InputFile f(m_Path.c_str());
    const Imf::Header& header = f.header();

    /* Basic Metadata */
    m["filepath"] = m_Path;

    int channelCount = 0;
    const Imf::ChannelList& channels = header.channels();

    Imf::PixelType pixelType = Imf::PixelType::HALF;
    for (Imf::ChannelList::ConstIterator it = channels.begin(); it != channels.end(); ++it)
    {
        channelCount++;
        pixelType = it.channel().type;
    }

    /**
     * UINT  = 0, // unsigned int (32 bit)
     * HALF  = 1, // half (16 bit floating point)
     * FLOAT = 2, // float (32 bit floating point)
     */
    if (pixelType == Imf::PixelType::HALF)
        m["pixelType"] = "16 bit floating point";
    else if (pixelType == Imf::PixelType::FLOAT)
        m["pixelType"] = "32 bit floating point";
    else
        m["pixelType"] = "32 bit";

    Imath::Box2i dw = header.dataWindow();
    int width = dw.max.x - dw.min.x + 1;
    int height = dw.max.y - dw.min.y + 1;

    m["width"] = std::to_string(width);
    m["height"] = std::to_string(height);
    m["channels"] = std::to_string(channelCount);

    // const Imf::Compression& compression = header.compression();
    // std::string compressionType;
    // Imf::getCompressionNameFromId(compression, compressionType);
    // m["compression"] = compressionType;

    /* Specific metadata */
    for (Imf::Header::ConstIterator it = header.begin(); it != header.end(); ++it)
    {
        const Imf::Attribute& attr = it.attribute();

        if (std::strcmp(attr.typeName(), "string") == 0)
        {
            const Imf::StringAttribute& sattr = static_cast<const Imf::StringAttribute&>(attr);
            m[it.name()] = sattr.value();
        }
        else if (std::strcmp(attr.typeName(), "float") == 0)
        {
            const Imf::FloatAttribute& fattr = static_cast<const Imf::FloatAttribute&>(attr);
            m[it.name()] = std::to_string(fattr.value());
        }
        else if (std::strcmp(attr.typeName(), "int") == 0)
        {
            const Imf::IntAttribute& iattr = static_cast<const Imf::IntAttribute&>(attr);
            m[it.name()] = std::to_string(iattr.value());
        }
    }

    return m;
}

VOID_NAMESPACE_CLOSE
