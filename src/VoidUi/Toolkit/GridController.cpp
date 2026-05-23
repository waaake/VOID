// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QLabel>

/* Internal */
#include "GridController.h"
#include "VoidUi/Player/PlayerBridge.h"

VOID_NAMESPACE_OPEN

GridController::GridController(QWidget* parent)
    : TranslucentDialog(parent)
{
    Build();
    Setup();
    Connect();
}

GridController::~GridController()
{
    m_Validator->deleteLater();
    delete m_Validator;
    m_Validator = nullptr;

    m_Layout->deleteLater();
    delete m_Layout;
    m_Layout = nullptr;
}

void GridController::Build()
{
    m_Layout = new QGridLayout(this);

    m_Layout->addWidget(new QLabel("Edit Render Grid", this), 0, 0, 1, 4);
    m_Layout->addWidget(new QLabel("Rows", this), 1, 0, 1, 2);
    m_Layout->addWidget(new QLabel("Columns", this), 1, 2, 1, 2);

    m_ColumnsEdit = new QLineEdit(this);
    m_RowsEdit = new QLineEdit(this);

    m_Layout->addWidget(m_RowsEdit, 2, 0, 1, 2);
    m_Layout->addWidget(m_ColumnsEdit, 2, 2, 1, 2);
}

void GridController::Connect()
{
    connect(m_ColumnsEdit, &QLineEdit::editingFinished, this, [this]() -> void
    {
        QString t = m_ColumnsEdit->text();
        if (!t.isEmpty())
        {
            _PlayerBridge.SetGridColumns(t.toInt());
            m_RowsEdit->setPlaceholderText(QString::number(_PlayerBridge.GridRows()));
            m_RowsEdit->clear();
        }
    });
    connect(m_ColumnsEdit, &QLineEdit::returnPressed, this, [this]() -> void
    {
        QString t = m_ColumnsEdit->text();
        if (!t.isEmpty())
        {
            _PlayerBridge.SetGridColumns(t.toInt());
            m_RowsEdit->setPlaceholderText(QString::number(_PlayerBridge.GridRows()));
            m_RowsEdit->clear();
        }
    });

    connect(m_RowsEdit, &QLineEdit::editingFinished, this, [this]() -> void
    {
        QString t = m_RowsEdit->text();
        if (!t.isEmpty())
        {
            _PlayerBridge.SetGridRows(t.toInt());
            m_ColumnsEdit->setPlaceholderText(QString::number(_PlayerBridge.GridColumns()));
            m_ColumnsEdit->clear();
        }
    });
    connect(m_RowsEdit, &QLineEdit::returnPressed, this, [this]() -> void
    {
        QString t = m_RowsEdit->text();
        if (!t.isEmpty())
        {
            _PlayerBridge.SetGridRows(t.toInt());
            m_ColumnsEdit->setPlaceholderText(QString::number(_PlayerBridge.GridColumns()));
            m_ColumnsEdit->clear();
        }
    });
}

void GridController::Setup()
{
    m_Validator = new QIntValidator(1, 20, this);

    m_RowsEdit->setPlaceholderText(QString::number(_PlayerBridge.GridRows()));
    m_ColumnsEdit->setPlaceholderText(QString::number(_PlayerBridge.GridColumns()));

    m_RowsEdit->setValidator(m_Validator);
    m_ColumnsEdit->setValidator(m_Validator);
}

VOID_NAMESPACE_CLOSE
