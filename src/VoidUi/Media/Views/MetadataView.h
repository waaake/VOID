// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_MEDIA_METADATA_VIEW_H
#define _VOID_MEDIA_METADATA_VIEW_H

/* STD */
#include <map>

/* Qt */
#include <QLayout>
#include <QPushButton>
#include <QTreeView>
#include <QWidget>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Media/MediaClip.h"
#include "VoidUi/Media/Models/MetadataModel.h"

VOID_NAMESPACE_OPEN

class MetadataTree : public QTreeView
{
public:
    explicit MetadataTree(QWidget* parent = nullptr);
    ~MetadataTree();

    inline void SetMetadata(const std::map<std::string, std::string>& metadata) { m_Metadata->SetMetadata(metadata); }
    inline void Clear() { m_Metadata->Clear(); }

private: /* Members */
    MetadataModel* m_Metadata;

private: /* Methods */
    void Setup();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_METADATA_VIEW_H
