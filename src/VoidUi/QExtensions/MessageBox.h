// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_Q_EXT_MESSAGE_BOX_H
#define _VOID_Q_EXT_MESSAGE_BOX_H

/* Qt */
#include <QMessageBox>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

/**
 * A Generic Save Message Box
 */
class SaveMessageBox : public QMessageBox
{
    Q_OBJECT

public:
    SaveMessageBox(QWidget* parent = nullptr);
    SaveMessageBox(const QString& title, const QString& text, QWidget* parent = nullptr);

    QMessageBox::StandardButton Prompt();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_Q_EXT_MESSAGE_BOX_H
