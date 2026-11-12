// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_OPEN_EXR_READER_H
#define _VOID_OPEN_EXR_READER_H

/* STD */
#include <vector>

/* Internal */
#include "Definition.h"
#include "PixReader.h"
#include "MediaReader.h"

VOID_NAMESPACE_OPEN

class VOID_API OpenEXRReader : public VoidPixReader
{
public:
    OpenEXRReader(const std::string& path, v_frame_t framenumber = 0);
    ~OpenEXRReader();

    void Read(const std::string& path, v_frame_t frame, FloatImage& image) override;
    void Read() override;
    void ReadThumbnail(const std::string& path, v_frame_t frame, UInt8Image& image) override;
    void Clear() override;
    const std::map<std::string, std::string> Metadata() const override;
    MediaColor MediaColorInfo() const override;
};

class VOID_API OpenEXRMediaReader : public MediaReader
{
public:
    using MediaReader::MediaReader;

    bool Read(const std::string& path, v_frame_t framenumber, UInt8Image& image) override;
    bool Read(const std::string& path, v_frame_t framenumber, FloatImage& image) override;

    MediaColor* ColorInfo() override { return &m_ColorInfo; }

    /// Only reads images
    bool IsMovieFormat() const { return false; }
    std::map<std::string, std::string> Metadata() const override;

private:
    MediaColor m_ColorInfo;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_OPEN_EXR_READER_H
