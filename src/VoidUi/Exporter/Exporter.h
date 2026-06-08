// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _PLAYER_EXPORTER_H
#define _PLAYER_EXPORTER_H

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
    ExportMediaFramesTask(const SharedMediaClip& media, const MediaExportDescriptor& descriptor, const EncodeSpec& spec, const MFrameRange& range);
    inline std::string Label() const override { return m_Descriptor.entry.TemplatedName(); }

protected:
    bool Work() override;

private:
    std::weak_ptr<MediaClip> m_Media;
    MediaExportDescriptor m_Descriptor;
    EncodeSpec m_Spec;
    MFrameRange m_Range;
};

VOID_NAMESPACE_CLOSE

#endif // _PLAYER_EXPORTER_H
