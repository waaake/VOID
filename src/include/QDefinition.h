#ifndef _VOID_Q_DEFINITIONS_H
#define _VOID_Q_DEFINITIONS_H

/* Qt */
#include <QObject>

/* Internal */
#include "Definition.h"

/**
 * Qt6 compat means that the changes are compatible with Qt6 while still being in qt5
 * qt5.15 started moving towards a compatible API
 */
#define _QT6_COMPAT QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
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
