// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _MEDIA_TAG_WIDGET_H
#define _MEDIA_TAG_WIDGET_H

/* Internal */
#include "Definition.h"
#include "VoidObjects/Media/MediaClip.h"
#include "VoidToolbox/Editor/TagEditor.h"

VOID_NAMESPACE_OPEN

class MediaTagWidget : public TagWidget
{
public:
    MediaTagWidget(const QModelIndex& index, QWidget* parent = nullptr);
    ~MediaTagWidget();

private:
    QModelIndex m_MediaIndex;
    TagMetadataModel* m_Metadata;
};

class MediaTagEditor : public TagEditor
{
public:
    MediaTagEditor(const SharedMediaClip& clip, const QModelIndex& index, QWidget* parent = nullptr);

private:
    std::weak_ptr<MediaClip> m_Media;
    QModelIndex m_Index;

private:
    void Setup();
    void RemoveSelected();
};

VOID_NAMESPACE_CLOSE

#endif // _MEDIA_TAG_WIDGET_H
