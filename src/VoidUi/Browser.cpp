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

VOID_NAMESPACE_CLOSE