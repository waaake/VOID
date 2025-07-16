#ifndef _VOID_DOCKER_H
#define _VOID_DOCKER_H

/* Qt */
#include <QDockWidget>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class VoidDocker : public QDockWidget
{
    Q_OBJECT

public:
    VoidDocker(QWidget* parent = nullptr);
    VoidDocker(const std::string& title, QWidget*parent = nullptr);
    virtual ~VoidDocker();
    
    /* Helpers */

    /*
     * Adjusts the Dock panel's closable property
     * If true, the dock panel gets a close button and can be closed from it
     * false would get rid of the close button
     */
    void SetClosable(const bool closable);

};

VOID_NAMESPACE_CLOSE

#endif // _VOID_DOCKER_H
