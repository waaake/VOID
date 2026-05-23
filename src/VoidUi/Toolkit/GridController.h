// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _GRID_CONTROLLER_TOOLKIT_H
#define _GRID_CONTROLLER_TOOLKIT_H

/* Qt */
#include <QLayout>
#include <QLineEdit>
#include <QValidator>

/* Internal */
#include "Definition.h"
#include "VoidUi/QExtensions/Dialog.h"

VOID_NAMESPACE_OPEN

class GridController : public TranslucentDialog
{
    Q_OBJECT

public:
    GridController(QWidget* parent = nullptr);
    ~GridController();

private: /* Members */
    QLineEdit* m_ColumnsEdit;
    QLineEdit* m_RowsEdit;
    QIntValidator* m_Validator;

    QGridLayout* m_Layout;

private: /* Methods */
    void Build();
    void Connect();
    void Setup();
};

VOID_NAMESPACE_CLOSE

#endif // _GRID_CONTROLLER_TOOLKIT_H
