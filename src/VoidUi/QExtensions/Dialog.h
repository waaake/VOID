// Copyright (c) 2025 waaake
// Licensed under the MIT License

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
