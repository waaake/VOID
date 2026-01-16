// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Browser.h"

VOID_NAMESPACE_OPEN

MediaBrowser::MediaBrowser(QWidget* parent)
    : MediaFileDialog(parent)
{
}

MediaBrowser::~MediaBrowser()
{

}

bool MediaBrowser::Browse()
{
    SetFileMode(MediaFileDialog::FileMode::ExistingFile);
    return exec();
}

bool MediaBrowser::BrowseDirectory()
{
    SetFileMode(MediaFileDialog::FileMode::Directory);
    return exec();
}

VOID_NAMESPACE_CLOSE
