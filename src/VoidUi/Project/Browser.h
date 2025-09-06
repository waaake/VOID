// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_PROJECT_BROWSER_H
#define _VOID_PROJECT_BROWSER_H

/* Qt */
#include <QFileDialog>

/* Internal */
#include "Definition.h"
#include "VoidCore/Serialization.h"

VOID_NAMESPACE_OPEN

struct VoidFileDescriptor
{
    std::string path;
    std::string name;
    EtherFormat::Type type;
};

class VoidProjectBrowser : public QFileDialog
{
public:
    VoidProjectBrowser(QWidget* parent = nullptr);
    virtual ~VoidProjectBrowser();

    /*
     * Begin browsing
     * Calls exec() which allows user to select on an entry from filesystem
     * will wait executing other instructions below and returns back a bool value telling if the browse
     * was successful or not.
     */
    [[nodiscard]] bool Browse();
    [[nodiscard]] bool Save();

    VoidFileDescriptor File() const;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PROJECT_BROWSER_H
