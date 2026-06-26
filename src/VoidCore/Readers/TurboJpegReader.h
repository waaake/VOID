// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_TURBO_JPEG_READER_H
#define _VOID_TURBO_JPEG_READER_H

/* STD */
#include <cmath>
#include <vector>

/* Internal */
#include "Definition.h"
#include "PixReader.h"

VOID_NAMESPACE_OPEN

class VOID_API TurboJpegReader : public VoidPixReader
{
public:
    TurboJpegReader(const std::string& path, v_frame_t framenumer = 0);
    ~TurboJpegReader();

    void ReadThumbnail(const std::string& path, v_frame_t frame, UInt8Image& image) override;
    void Read(const std::string& path, v_frame_t frame, FloatImage& image) override;
    void Clear() override;
    const std::map<std::string, std::string> Metadata() const override;

private: /* Members */
    std::vector<unsigned char> m_Pixels;

private: /* Methods */
    inline static float Linear(float pixel) { return (pixel <= 0.04045f) ? pixel / 12.92f : powf((pixel + 0.055f) / 1.055f, 2.4f); }
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_TURBO_JPEG_READER_H
