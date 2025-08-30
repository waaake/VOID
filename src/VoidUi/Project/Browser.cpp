// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <filesystem>

/* Internal */
#include "Browser.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

VoidProjectBrowser::VoidProjectBrowser(QWidget* parent)
    : QFileDialog(parent)
{
    setOption(QFileDialog::DontUseNativeDialog);
    setNameFilter("VOID State Files (*.vois)");
}

VoidProjectBrowser::~VoidProjectBrowser()
{
}

bool VoidProjectBrowser::Browse()
{
    setDefaultSuffix("");
    setAcceptMode(QFileDialog::AcceptOpen);
    setFileMode(QFileDialog::ExistingFile);
    return exec();
}

bool VoidProjectBrowser::Save()
{
    setAcceptMode(QFileDialog::AcceptSave);
    setFileMode(QFileDialog::AnyFile);

    return exec();
}

VoidFileDescriptor VoidProjectBrowser::File() const
{
    QString filepath = selectedFiles().first();

    /* Add Extension if not already added */
    if (!filepath.endsWith(".vois", Qt::CaseSensitive))
    {
        filepath += ".vois";
    }

    std::filesystem::path p(filepath.toStdString());
    return { p.string(), p.stem().string() };
}

VOID_NAMESPACE_CLOSE
