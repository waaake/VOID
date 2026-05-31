// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Browser.h"

VOID_NAMESPACE_OPEN

const QString images = "Image Media (*.png *.jpg)";
const QString movies = "Movie Media (*.mov)";

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

/// MediaExport Browser

MediaExportBrowser::MediaExportBrowser(QWidget* parent)
    : QFileDialog(parent)
{
    setOption(QFileDialog::DontUseNativeDialog);
    setNameFilters({ images, movies });
}

bool MediaExportBrowser::Save()
{
    setAcceptMode(QFileDialog::AcceptSave);
    setFileMode(QFileDialog::AnyFile);

    return exec();
}

MediaExportDescriptor MediaExportBrowser::File() const
{
    QString filepath = selectedFiles().first();
    QString filter = selectedNameFilter();

    WriterType type = filter == images ? WriterType::Image : WriterType::Movie;
    return { type, MEntry(filepath.toStdString()) };
}

VOID_NAMESPACE_CLOSE
