// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <filesystem>

/* Internal */
#include "Browser.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

const QString ascii = "VOID Element State (*.ether)";
const QString binary = "VOID Dark State (*.nether)";

VoidProjectBrowser::VoidProjectBrowser(QWidget* parent)
    : QFileDialog(parent)
{
    setOption(QFileDialog::DontUseNativeDialog);
    setNameFilters({ ascii, binary });
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
    QString filter = selectedNameFilter();

    EtherFormat::Type type;

    if (filter == ascii)
    {
        type = EtherFormat::Type::ASCII;

        /* Add Extension if not already added */
        if (!filepath.endsWith(EtherFormat::AsciiExt, Qt::CaseSensitive))
            filepath += EtherFormat::AsciiExt;
    }
    else
    {
        type = EtherFormat::Type::BINARY;

        /* Add Extension if not already added */
        if (!filepath.endsWith(EtherFormat::BinaryExt, Qt::CaseSensitive))
            filepath += EtherFormat::BinaryExt;
    }

    std::filesystem::path p(filepath.toStdString());
    return { p.string(), p.stem().string(), type };
}

VOID_NAMESPACE_CLOSE
