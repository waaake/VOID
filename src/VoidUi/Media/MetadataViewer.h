// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_MEDIA_METADATA_VIEWER_H
#define _VOID_MEDIA_METADATA_VIEWER_H

/* STD */
#include <map>

/* Qt */
#include <QDropEvent>
#include <QLayout>
#include <QPushButton>
#include <QTreeView>
#include <QWidget>

/* Internal */
#include "Definition.h"
#include "VoidUi/Media/MediaBridge.h"
#include "VoidUi/Media/Views/MetadataView.h"

VOID_NAMESPACE_OPEN

class MetadataViewer : public QWidget
{
public:
    explicit MetadataViewer(QWidget* parent = nullptr);
    ~MetadataViewer();

    /* Set Metadata on the viewer from the media clip */
    inline void SetFromMedia(const SharedMediaClip& clip) { m_Tree->SetMetadata(clip->Metadata()); }
    /* Set the Metadata on the viewer from the provided map of data */
    inline void SetMetadata(const std::map<std::string, std::string>& metadata) { m_Tree->SetMetadata(metadata); }

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private: /* Members */
    QVBoxLayout* m_Layout;
    QHBoxLayout* m_ButtonLayout;
    QPushButton* m_ClearButton;
    MetadataTree* m_Tree;

private: /* Methods */
    void Build();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_METADATA_VIEWER_H
