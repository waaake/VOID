// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Browser.h"

VOID_NAMESPACE_OPEN

VoidMediaBrowser::VoidMediaBrowser(QWidget* parent)
    : QFileDialog(parent)
{

}

VoidMediaBrowser::~VoidMediaBrowser()
{

}

bool VoidMediaBrowser::Browse()
{
    return exec();
}

std::string VoidMediaBrowser::GetSelectedFile() const
{
    QStringList files = selectedFiles();

    /* Empty Media struct */
    if (files.empty())
        return "";

    return files.first().toStdString();
}

MediaStruct VoidMediaBrowser::GetMediaStruct() const
{
    QStringList files = selectedFiles();

    /* Empty Media struct */
    if (files.empty())
        return MediaStruct();

    /* Return the MediaStruct constructed from the selected file */
    return MediaStruct::FromFile(files.first().toStdString());
}

VOID_NAMESPACE_CLOSE
