#ifndef _VOID_MEDIA_BROWSER_H
#define _VOID_MEDIA_BROWSER_H

/* Qt */
#include <QFileDialog>

/* Internal */
#include "Definition.h"
#include "VoidCore/MediaFilesystem.h"

VOID_NAMESPACE_OPEN

class VoidMediaBrowser : public QFileDialog
{
public:
    VoidMediaBrowser(QWidget* parent = nullptr);
    virtual ~VoidMediaBrowser();

    /*
     * Begin browsing
     * Calls exec() which allows user to select on an entry from filesystem
     * will wait executing other instructions below and returns back a bool value telling if the browse
     * was successful or not.
     */
    [[nodiscard]] bool Browse();     

    inline std::string GetDirectory() const { return directory().absolutePath().toStdString(); }
    MediaStruct GetMediaStruct() const;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_BROWSER_H