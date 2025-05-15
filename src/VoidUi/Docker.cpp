/* Internal */
#include "Docker.h"

VOID_NAMESPACE_OPEN

VoidDocker::VoidDocker(QWidget* parent)
    : QDockWidget(parent)
{
}

VoidDocker::VoidDocker(const std::string& title, QWidget* parent)
    : QDockWidget(title.c_str(), parent)
{
}

VoidDocker::~VoidDocker()
{
}

void VoidDocker::SetClosable(const bool closable)
{
    if (closable)
        setFeatures(features() & QDockWidget::DockWidgetClosable);
    else
        setFeatures(features() & ~QDockWidget::DockWidgetClosable);
}

VOID_NAMESPACE_CLOSE