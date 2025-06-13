#ifndef _VOID_Q_DEFINITIONS_H
#define _VOID_Q_DEFINITIONS_H

/* Qt */
#include <QObject>

/* Internal */
#include "Definition.h"

#define _QT6 QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)

/* Define type EnterEvent */
#if _QT6
typedef QEnterEvent EnterEvent;
#else
typedef QEvent EnterEvent;
#endif

/* Define type Point */
#if _QT6
typedef QPointF Point;
#else
typedef QPoint Point;
#endif

#endif // _VOID_Q_DEFINITIONS_H
