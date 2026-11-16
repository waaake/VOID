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

struct ChannelLayout
{
    enum class Type
    {
        RGBA,
        XYZ,
        UV,
        CRYPTOMATTE,
        OTHER
    };

    int indices[4] = { -1, -1, -1, -1 };
    int channels = 0;
    Type type;
};

struct EXRStream
{
    std::string name;
    std::vector<std::string> channels;
    ChannelLayout layout;

    bool Valid() const noexcept { return !channels.empty(); }
};

ChannelLayout GetChannelLayout(const std::vector<std::string>& channels)
{
    ChannelLayout layout;

    // We only want to process 4 channels, based on our rendering pipeline, so ensure we cap to it
    layout.channels = std::min(channels.size(), (std::size_t)4);

    static constexpr std::string_view rgba[] = { "r", "g", "b", "a" };
    static constexpr std::string_view xyz[] = { "x", "y", "z", "w" };
    static constexpr std::string_view uv[] = { "u", "v" };

    for (int i = 0; i < layout.channels; ++i)
    {
        std::string_view name = channels[i];
        const std::size_t dot = name.rfind(".");
        const std::string_view component = dot == std::string_view::npos ? name : name.substr(dot + 1);

        // RGBA validation
        for (int c = 0; c < 4; ++c)
        {
            if (Tools::equals_ignore_case(component, rgba[c]))
            {
                layout.indices[c] = i;
                layout.type = ChannelLayout::Type::RGBA;
                
                break;
            }
        }

        // XYZ validation
        for (int c = 0; c < 4; ++c)
        {
            if (Tools::equals_ignore_case(component, xyz[c]))
            {
                layout.indices[c] = i;
                layout.type = ChannelLayout::Type::XYZ;
            }
        }

        // UV validation
        for (int c = 0; c < 2; ++c)
        {
            if (Tools::equals_ignore_case(component, uv[c]))
            {
                layout.indices[c] = i;
                layout.type = ChannelLayout::Type::UV;
            }
        }
    }

    return layout;
}

bool is_blank(std::span<float> data)
{
    auto _pred = [](const float& _f) -> bool { return _f > 0.0f; };
    auto it = std::find_if(data.begin(), data.end(), _pred);
    return it == data.end();
}

std::map<std::string, std::vector<std::string>> HeaderChannels(const Imf::Header& header)
{
    std::map<std::string, std::vector<std::string>> names;

    const std::string prefix = header.hasName() ? header.name() + "." : "";
    const Imf::ChannelList& channels = header.channels();

    static constexpr std::string_view rgba[] = {
        "r", "g", "b", "a",
        "red", "green", "blue", "alpha"
    };
    static constexpr std::string_view xyz[] = { "x", "y", "z", "w" };

    for (Imf::ChannelList::ConstIterator it = channels.begin(); it != channels.end(); ++it)
    {
        const std::string_view channel(it.name());
        const std::size_t pos = channel.rfind(".");

        if (pos != std::string_view::npos && pos != 0)
        {
            std::string key;
            key.reserve(prefix.size() + pos);
            key.append(prefix);
            key.append(channel.data(), pos);

            names[std::move(key)].emplace_back(channel);
            continue;
        }

        std::string_view group = "Other";
        for (const auto name : rgba)
        {
            if (Tools::equals_ignore_case(channel, name))
            {
                group = "RGBA";
                break;
            }
        }

        if (group == "Other")
        {
            for (const auto name : xyz)
            {
                if (Tools::equals_ignore_case(channel, name))
                {
                    group = "XYZ";
                    break;
                }
            }
        }

        std::string key;
        key.reserve(prefix.size() + group.size());
        key.append(prefix);
        key.append(group);

        names[std::move(key)].emplace_back(channel);
    }

    if (header.hasName() && names.size() == 1)
    {
        auto node = names.extract(names.begin());
        node.key() = header.name();
        names.insert(std::move(node));
    }

    return names;
}

std::vector<EXRStream> GetStreams(const Imf::Header& header)
{
    const std::map<std::string, std::vector<std::string>> groups = HeaderChannels(header);
    const std::string prefix = header.hasName() ? header.name() + "." : "";

    std::vector<EXRStream> streams;
    streams.reserve(groups.size());

    auto _addStream = [&](const std::string& name)
    {
        const auto it = groups.find(name);
        if (it == groups.end()) return;

        EXRStream stream;
        stream.name = name;
        stream.channels = it->second;

        streams.emplace_back(std::move(stream));
    };

    // Order based on the common ones
    const std::string rgba = prefix + "RGBA";
    const std::string xyz = prefix + "XYZ";
    if (groups.contains(rgba))
        _addStream(rgba);
    else
    {
        for (const auto& [name, _] : groups)
        {
            const std::string lower = Tools::to_lower(name);
            if (lower.find(".rgb") != std::string::npos || lower.ends_with(".combined") || lower == "combined")
            {
                _addStream(name);
                break;
            }
        }
    }

    // Add if present
    _addStream(xyz);

    for (const auto& [name, _] : groups)
    {
        if (name == rgba || name == xyz)
            continue;

        if (!streams.empty() && streams.back().name == name)
            continue;
        
        _addStream(name);
    }

    return streams;
}

// channels is copied, need a better way around this
std::vector<std::string> HeaderStreams2(const Imf::Header& header, std::map<std::string, std::vector<std::string>> channels)
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

std::vector<std::string> HeaderStreams(const Imf::Header& header, const std::map<std::string, std::vector<std::string>>& channels)
{
    std::vector<std::string> streams;
    streams.reserve(channels.size());

    if (channels.contains("RGBA"))
    {
        streams.emplace_back("RGBA");
    }
    else
    {
        for (const auto& [name, _] : channels)
        {
            const std::string layer = Tools::to_lower(name);

            if (layer.find(".rgb") != std::string::npos || layer.ends_with(".combined") || layer == "combined")
            {
                streams.emplace_back(name);
                break;
            }
        }
    }

    if (channels.contains("XYZ"))
        streams.emplace_back("XYZ");
    
    for (const auto& [name, _] : channels)
    {
        if (name == "RGBA" || name == "XYZ")
            continue;
        
        if (!streams.empty() && streams.back() == name)
            continue;
        
        streams.emplace_back(name);
    }

    return streams;
}

bool FetchStream(const Imf::Header& header, const std::string& layer, EXRStream& out)
{
    std::vector<EXRStream> streams = GetStreams(header);
    for (auto& stream : streams)
    {
        if (stream.name == layer)
        {
            // std::swap(out, stream);
            out = std::move(stream);
            out.layout = GetChannelLayout(out.channels);
            return true;
        }
    }

    return false;
}

void FirstStream(const Imf::Header& header, EXRStream& out)
{
    std::vector<EXRStream> streams = GetStreams(header);
    if (streams.empty())
        return;
    
    out = std::move(streams[0]);
    out.layout = GetChannelLayout(out.channels);
}

bool ReadInterleaved(Imf::InputPart& part, const EXRStream& stream, FloatImage& image)
{
    const Imf::Header& header = part.header();
    const Imath::Box2i dataWindow = header.dataWindow();

    image->width = int64_t(dataWindow.max.x) - dataWindow.min.x + 1;
    image->height = int64_t(dataWindow.max.y) - dataWindow.min.y + 1;

    const size_t channels = stream.channels.size();

    image->type = VOID_GL_FLOAT;
    image->channels = static_cast<int>(channels);

    switch (channels)
    {
        case 1:
            image->format = VOID_GL_R32F;
            image->pixfmt = VOID_GL_RED;
            break;
        case 2:
            image->format = VOID_GL_RG32F;
            image->pixfmt = VOID_GL_RG;
            break;
        case 3:
            image->format = VOID_GL_RGB;
            image->pixfmt = VOID_GL_RGB;
            break;
        case 4:
        default:
            image->format = VOID_GL_RGBA32F;
            image->pixfmt = VOID_GL_RGBA;
            break;
    }

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
        const std::string& name = stream.channels[c];
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

        // This will give the actual index of the channel at position c
        // e.g. RGBA is the layout we're expecting, but the channels in the exr may be
        // ABRG, so when we ask the index of the channel at position 0 i.e. channel A, it will return us 4
        // this ensures that the interleaved data that we have corresponds to the correct RGBA position or mapped accordingly
        int chindex = stream.layout.indices[c];
        float* channelPtr = image->buffer._buf.data() + chindex;
        frameBuffer.insert(name, Imf::Slice::Make(Imf::FLOAT, channelPtr, dataWindow, pixelStride, rowStride, 1, 1, 0.0));
    }

    part.setFrameBuffer(frameBuffer);

    // One full-frame request. OpenEXR handles the underlying
    // compression blocks and decompression scheduling
    part.readPixels(dataWindow.min.y, dataWindow.max.y);
    return true;
}

} // namespace

// OpenEXRReader::OpenEXRReader(const std::string& path, v_frame_t framenumber)
//     : VoidPixReader(path, framenumber)
// {
//     Imf::setGlobalThreadCount(16);
// }

// OpenEXRReader::~OpenEXRReader()
// {
//     Clear();
// }

// void OpenEXRReader::Clear()
// {
//     m_Image->Clear();
// }

// void OpenEXRReader::ReadThumbnail(const std::string& path, v_frame_t frame, UInt8Image& image)
// {
//     Imf::RgbaInputFile f(path.c_str(), Tools::thread_count());
//     // Image Specs
//     Imath::Box2i dw = f.dataWindow();
//     image->width = (dw.max.x - dw.min.x) + 1;
//     image->height = (dw.max.y - dw.min.y) + 1;

//     /* To Get the channels -> Read through the header */
//     // const Imf::Header& header = f.header();
//     // const Imf::ChannelList& channels = header.channels();

//     // /*
//     //  * TODO: Check if we have a better way of reading channels rather than iterating here ?
//     //  * Update the channel count by reading the channels
//     //  */
//     // for (Imf::ChannelList::ConstIterator it = channels.begin(); it != channels.end(); ++ it)
//     //     m_Channels++;

//     // Force 4 Channels
//     image->channels = 4;

//     // Image Specs
//     m_Image->width = image->width;
//     m_Image->height = image->height;
//     m_Image->channels = image->channels;

//     // VOID_LOG_INFO("EXRImage ( Width: {0}, Height: {1}, Channels: {2} )", image->width, image->height, image->channels);

//     Imf::Array2D<Imf::Rgba> pixels(image->height, image->width);

//     // Read the Pixel data onto the buffer
//     f.setFrameBuffer(&pixels[0][0] - dw.min.x - dw.min.y * image->width, 1, image->width);
//     f.readPixels(dw.min.y, dw.max.y);

//     image->buffer.Resize(image->width * image->height * image->channels);
//     const int64_t pixelcount = static_cast<int64_t>(m_Image->width) * m_Image->height;
//     const Imf::Rgba* source = &pixels[0][0];
//     unsigned char* im = image->buffer.Data();

//     tbb::parallel_for(tbb::blocked_range<int64_t>(0, pixelcount), [&](const tbb::blocked_range<int64_t>& r) -> void
//     {
//         for (int64_t i = r.begin(); i != r.end(); ++i)
//         {
//             const Imf::Rgba pixel = source[i];
//             const int index = i * 4;

//             im[index + 0] = static_cast<unsigned char>(std::clamp((float)pixel.r, 0.f, 1.f) * 255.f);
//             im[index + 1] = static_cast<unsigned char>(std::clamp((float)pixel.g, 0.f, 1.f) * 255.f);
//             im[index + 2] = static_cast<unsigned char>(std::clamp((float)pixel.b, 0.f, 1.f) * 255.f);
//             im[index + 3] = static_cast<unsigned char>(std::clamp((float)pixel.a, 0.f, 1.f) * 255.f);
//         }
//     });
// }

// void OpenEXRReader::Read(const std::string& path, v_frame_t frame, FloatImage& image)
// {
//     Imf::MultiPartInputFile input(path.c_str());

//     int index = -1;
//     std::vector<std::string> channelsToLoad;

//     for (int i = 0; i < input.parts(); ++i)
//     {
//         const Imf::Header& header = input.header(i);

//         const std::map<std::string, std::vector<std::string>> channels = HeaderChannels(header);
//         const std::vector<std::string> streams = HeaderStreams(header, channels);

//         for (const auto& stream : streams)
//         {
//             if (stream == "RGBA")
//             {
//                 channelsToLoad.clear();
//                 channelsToLoad = channels.at(stream);

//                 if (channelsToLoad.empty())
//                     break;

//                 SortChannels(channelsToLoad);

//                 index = i;
//                 break;
//             }
//         }

//         if (index >= 0)
//             break;
//     }

//     if (index < 0)
//     {
//         VOID_LOG_ERROR("No RGBA stream found in {}", m_Path);
//         return;
//     }

//     if (channelsToLoad.empty() || channelsToLoad.size() > 4)
//     {
//         VOID_LOG_ERROR("Invalid channel count");
//         return;
//     }

//     image->type = VOID_GL_FLOAT;
//     Imf::InputPart part(input, index);

//     // Read Success
//     if (ReadInterleaved(part, channelsToLoad, image))
//         return;

//     /// There maybe cases, this is partially read and we don't want to display partial information on the viewer at the moment
//     /// Better is that we clear the image buffer in that case
//     m_Image->Clear();
// }

// void OpenEXRReader::Read()
// {
//     Imf::MultiPartInputFile input(m_Path.c_str());

//     int index = -1;
//     std::vector<std::string> channelsToLoad;

//     for (int i = 0; i < input.parts(); ++i)
//     {
//         const Imf::Header& header = input.header(i);

//         const std::map<std::string, std::vector<std::string>> channels = HeaderChannels(header);
//         const std::vector<std::string> streams = HeaderStreams(header, channels);

//         for (const auto& stream : streams)
//         {
//             if (stream == "RGBA")
//             {
//                 channelsToLoad.clear();
//                 channelsToLoad = channels.at(stream);

//                 if (channelsToLoad.empty())
//                     break;

//                 SortChannels(channelsToLoad);

//                 index = i;
//                 break;
//             }
//         }

//         if (index >= 0)
//             break;
//     }

//     if (index < 0)
//     {
//         VOID_LOG_ERROR("No RGBA stream found in {}", m_Path);
//         return;
//     }

//     if (channelsToLoad.empty() || channelsToLoad.size() > 4)
//     {
//         VOID_LOG_ERROR("Invalid channel count");
//         return;
//     }

//     m_Image->type = VOID_GL_FLOAT;
//     Imf::InputPart part(input, index);

//     // Read Success
//     if (ReadInterleaved(part, channelsToLoad, m_Image))
//         return;

//     /// There maybe cases, this is partially read and we don't want to display partial information on the viewer at the moment
//     /// Better is that we clear the image buffer in that case
//     m_Image->Clear();
// }

// const std::map<std::string, std::string> OpenEXRReader::Metadata() const
// {
//     std::map<std::string, std::string> m;

//     Imf::InputFile f(m_Path.c_str());
//     const Imf::Header& header = f.header();

//     /* Basic Metadata */
//     m["filepath"] = m_Path;

//     int channelCount = 0;
//     const Imf::ChannelList& channels = header.channels();

//     Imf::PixelType pixelType = Imf::PixelType::HALF;
//     for (Imf::ChannelList::ConstIterator it = channels.begin(); it != channels.end(); ++it)
//     {
//         channelCount++;
//         pixelType = it.channel().type;
//     }

//     /**
//      * UINT  = 0, // unsigned int (32 bit)
//      * HALF  = 1, // half (16 bit floating point)
//      * FLOAT = 2, // float (32 bit floating point)
//      */
//     if (pixelType == Imf::PixelType::HALF)
//         m["pixelType"] = "16 bit floating point";
//     else if (pixelType == Imf::PixelType::FLOAT)
//         m["pixelType"] = "32 bit floating point";
//     else
//         m["pixelType"] = "32 bit";

//     Imath::Box2i dw = header.dataWindow();
//     int width = dw.max.x - dw.min.x + 1;
//     int height = dw.max.y - dw.min.y + 1;

//     m["width"] = std::to_string(width);
//     m["height"] = std::to_string(height);
//     m["channels"] = std::to_string(channelCount);

//     // const Imf::Compression& compression = header.compression();
//     // std::string compressionType;
//     // Imf::getCompressionNameFromId(compression, compressionType);
//     // m["compression"] = compressionType;

//     /* Specific metadata */
//     for (Imf::Header::ConstIterator it = header.begin(); it != header.end(); ++it)
//     {
//         const Imf::Attribute& attr = it.attribute();

//         if (std::strcmp(attr.typeName(), "string") == 0)
//         {
//             const Imf::StringAttribute& sattr = static_cast<const Imf::StringAttribute&>(attr);
//             m[it.name()] = sattr.value();
//         }
//         else if (std::strcmp(attr.typeName(), "float") == 0)
//         {
//             const Imf::FloatAttribute& fattr = static_cast<const Imf::FloatAttribute&>(attr);
//             m[it.name()] = std::to_string(fattr.value());
//         }
//         else if (std::strcmp(attr.typeName(), "int") == 0)
//         {
//             const Imf::IntAttribute& iattr = static_cast<const Imf::IntAttribute&>(attr);
//             m[it.name()] = std::to_string(iattr.value());
//         }
//     }

//     return m;
// }

// MediaColor OpenEXRReader::MediaColorInfo() const
// {
//     MediaColor color;
//     color.colorspace = Colorspace::LinearRec709; // Default
//     /// Need some more insight on reading exact colorspaces from EXR versions lesser than 3.4

//     Imf::InputFile f(m_Path.c_str());
//     const Imf::Header& header = f.header();

//     const Imf::IntAttribute* acesf = header.findTypedAttribute<Imf::IntAttribute>("acesImageContainerFlag");
//     if (acesf)
//     {
//         if (acesf->value() == 1)
//         {
//             color.colorspace = Colorspace::ACES2065_1;
//             color.transfer = TransferFunction::Linear;

//             color.name = "ACES2065-1";
//         }

//         color.colorspaceMetadata = true;
//     }

//     // TODO: check chromaticities for a decent match to figure out colorspace
//     // const Imf::ChromaticitiesAttribute* chroma = header.findTypedAttribute<Imf::ChromaticitiesAttribute>("chromatacities");

//     return color;
// }

OpenEXRMediaReader::OpenEXRMediaReader(const std::string& path)
    : MediaReader(path)
{
    Imf::setGlobalThreadCount(std::max(Tools::thread_count() / 2, 4U));
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

    // /*
    //  * TODO: Check if we have a better way of reading channels rather than iterating here ?
    //  * Update the channel count by reading the channels
    //  */
    // for (Imf::ChannelList::ConstIterator it = channels.begin(); it != channels.end(); ++ it)
    //     m_Channels++;

    // Force 4 Channels
    image->channels = 4;

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

    std::vector<std::string> channelsToLoad;
    const int parts = input.parts();
    if (!parts)
        return false;

    EXRStream stream;
    int index = 0;

    /// Default - if layer wasn't set yet
    if (m_Format.layer.empty())
    {
        FirstStream(input.header(0), stream);
    }
    else
    {
        for (int i = 0; i < parts; ++i)
        {
            if (FetchStream(input.header(i), m_Format.layer, stream))
            {
                index = i;
                break;
            }
        }
    }

    Imf::InputPart part(input, index);

    // Read Success
    if (ReadInterleaved(part, stream, image))
        return true;

    /// There maybe cases, this is partially read and we don't want to display partial information on the viewer at the moment
    /// Better is that we clear the image buffer in that case
    image->Clear();
    return false;
}

void OpenEXRMediaReader::FetchInfo()
{
    Imf::MultiPartInputFile f(m_Path.c_str());
    const int parts = f.parts();

    const Imf::Header& header = f.header(0);
    const Imath::Box2i dw = header.dataWindow();

    m_Format.SetResolution((dw.max.x - dw.min.x) + 1, (dw.max.y - dw.min.y) + 1);

    for (int i = 0; i < parts; ++i)
    {
        const Imf::Header& h = f.header(i);
        auto chan = HeaderChannels(h);
        m_Format.channels += static_cast<int>(chan.size());
        std::vector<std::string> streams = HeaderStreams(h, std::move(chan));

        m_Format.layers.reserve(m_Format.layers.size() + streams.size());
        m_Format.layers.insert(m_Format.layers.end(), std::make_move_iterator(streams.begin()), std::make_move_iterator(streams.end()));
    }

    m_Format.color.colorspace = Colorspace::LinearRec709; // Default
    /// Need some more insight on reading exact colorspaces from EXR versions lesser than 3.4
    const Imf::IntAttribute* acesf = header.findTypedAttribute<Imf::IntAttribute>("acesImageContainerFlag");
    if (acesf)
    {
        if (acesf->value() == 1)
        {
            m_Format.color.colorspace = Colorspace::ACES2065_1;
            m_Format.color.transfer = TransferFunction::Linear;

            m_Format.color.name = "ACES2065-1";
        }

        m_Format.color.colorspaceMetadata = true;
    }
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
