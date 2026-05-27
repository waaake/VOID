// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _Q_EXT_DIALOG_H
#define _Q_EXT_DIALOG_H

/* Qt */
#include <QDialog>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class TranslucentDialog : public QDialog
{
public:
    TranslucentDialog(QWidget* parent = nullptr);
    void paintEvent(QPaintEvent* event) override;
};

VOID_NAMESPACE_CLOSE

#endif // _Q_EXT_DIALOG_H
