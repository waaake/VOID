// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _PLAYER_EXPORTER_H
#define _PLAYER_EXPORTER_H

/* STD */
#include <vector>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Core/Task.h"
#include "VoidObjects/Media/MediaClip.h"
#include "VoidUi/Media/Browser.h"

VOID_NAMESPACE_OPEN

// Forward decl
class Player;

class ExportAnnotatedFramesTask : public Task
{
public:
    ExportAnnotatedFramesTask(const MediaExportDescriptor& descriptor, Player* player);

protected:
    bool Work() override;

private:
    Player* m_Player;
    MediaExportDescriptor m_Descriptor;
};

class ExportMediaFramesTask : public Task
{
public:
    ExportMediaFramesTask(const SharedMediaClip& media, const MediaExportDescriptor& descriptor, const EncodeSpec& spec, const MFrameRange& range, const std::string& colorspace);
    inline std::string Label() const override { return m_Descriptor.entry.TemplatedName(); }

protected:
    bool Work() override;

private: /* Members */
    std::weak_ptr<MediaClip> m_Media;
    MediaExportDescriptor m_Descriptor;
    EncodeSpec m_Spec;
    MFrameRange m_Range;
    std::string m_Colorspace;
    std::vector<float> m_Pixels;

private: /* Methods */
    void ProcessImage(const void* pixels, int width, int height, int channels, const ColorSpace& incolorspace);
};

VOID_NAMESPACE_CLOSE

#endif // _PLAYER_EXPORTER_H
