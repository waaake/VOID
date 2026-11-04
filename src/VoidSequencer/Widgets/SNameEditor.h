// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_NAME_EDITOR_H
#define _SEQUENCER_NAME_EDITOR_H

/* Qt */
#include <QLineEdit>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

/// @brief A line edit acting as an edit field to rename graphics items within the Sequencer View
class SNameEditor : public QLineEdit
{
    Q_OBJECT
public:
    explicit SNameEditor(QWidget* parent = nullptr);

signals:
    void accepted(const QString&);
    void cancelled();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_NAME_EDITOR_H
