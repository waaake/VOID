// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QMouseEvent>

/* Internal */
#include "MessageBox.h"

VOID_NAMESPACE_OPEN

SaveMessageBox::SaveMessageBox(const QString& title, const QString& text, QWidget* parent)
    : QMessageBox(QMessageBox::Warning, title, text, QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, parent)
{
}

SaveMessageBox::SaveMessageBox(QWidget* parent)
    : SaveMessageBox("Unsaved Changes", "Would you like to save the project before closing?", parent)
{
}

QMessageBox::StandardButton SaveMessageBox::Prompt()
{
    return static_cast<QMessageBox::StandardButton>(exec());
}

VOID_NAMESPACE_CLOSE
