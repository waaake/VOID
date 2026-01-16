// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_MEDIA_BROWSER_H
#define _VOID_MEDIA_BROWSER_H

/* Qt */
#include <QFileDialog>

/* Internal */
#include "Definition.h"
#include "FileDialog.h"
#include "VoidCore/Media/Filesystem.h"

VOID_NAMESPACE_OPEN

class MediaBrowser : public MediaFileDialog
{
public:
    MediaBrowser(QWidget* parent = nullptr);
    virtual ~MediaBrowser();

    /*
     * Begin browsing
     * Calls exec() which allows user to select on an entry from filesystem
     * will wait executing other instructions below and returns back a bool value telling if the browse
     * was successful or not.
     */
    [[nodiscard]] bool Browse();
    [[nodiscard]] bool BrowseDirectory();

    inline std::string SelectedDirectory() const { return SelectedPath().toStdString(); }
    std::string GetSelectedFile() const { return SelectedPath().toStdString(); }
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_BROWSER_H
