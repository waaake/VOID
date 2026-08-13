// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>

/* Internal */
#include "Window.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
{
}

void MainWindow::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.fillRect(rect(), palette().color(QPalette::Dark));
}

VOID_NAMESPACE_CLOSE
