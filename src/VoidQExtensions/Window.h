// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _Q_EXT_WINDOW_H
#define _Q_EXT_WINDOW_H

/* Qt */
#include <QMainWindow>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class VOID_API MainWindow : public QMainWindow
{
public:
    MainWindow(QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

protected:
    virtual void paintEvent(QPaintEvent* event) override;
};

VOID_NAMESPACE_CLOSE

#endif // _Q_EXT_WINDOW_H
