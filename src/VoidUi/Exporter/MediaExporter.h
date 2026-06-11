// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _MEDIA_EXPORTER_H
#define _MEDIA_EXPORTER_H

/* Internal */
#include "Definition.h"
#include "ExportOptions.h"
#include "VoidObjects/Media/MediaClip.h"

VOID_NAMESPACE_OPEN

class MediaExporter : public ExportOptions
{
public:
    MediaExporter(const SharedMediaClip& media, QWidget* parent = nullptr);
    ~MediaExporter();

private:
    SharedMediaClip m_Media;

protected: /* Methods */
    void Setup();
    void Export();
    bool Validate();
};

VOID_NAMESPACE_CLOSE

#endif // _MEDIA_EXPORTER_H
