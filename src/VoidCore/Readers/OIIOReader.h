// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_OIIO_READER_H
#define _VOID_OIIO_READER_H

/* STD */
#include <vector>

/* Internal */
#include "Definition.h"
#include "PixReader.h"

VOID_NAMESPACE_OPEN

class VOID_API OIIOPixReader : public VoidPixReader
{
public:
    OIIOPixReader(const std::string& path, v_frame_t framenumer = 0);
    ~OIIOPixReader();

    void Read(const std::string& path, v_frame_t frame, FloatImage& image) override;
    void ReadThumbnail(const std::string& path, v_frame_t frame, UInt8Image& image) override;

    void Clear() override;
    const std::map<std::string, std::string> Metadata() const override;

private:
    // std::vector<unsigned char> m_TPixels;
    std::vector<unsigned char> m_Pixels;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_OIIO_READER_H
