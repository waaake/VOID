// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_RULER_WIDGET_H
#define _SEQUENCER_RULER_WIDGET_H

/* Qt */
#include <QWidget>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class SRulerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SRulerWidget(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event);
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_RULER_WIDGET_H
