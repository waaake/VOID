#ifndef _VOID_Q_DEFINITIONS_H
#define _VOID_Q_DEFINITIONS_H

/* Qt */
#include <QObject>

/* Internal */
#include "Definition.h"

#define _QT6 QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)

#if _QT6
typedef QEnterEvent EnterEvent;
#else
typedef QEvent EnterEvent;
#endif

#endif // _VOID_Q_DEFINITIONS_H
