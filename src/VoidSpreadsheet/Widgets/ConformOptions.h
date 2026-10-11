// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _CONFORM_OPTIONS_DIALOG_H
#define _CONFORM_OPTIONS_DIALOG_H

/* STD */
#include <vector>

/* Qt */
#include <QCheckBox>
#include <QDialog>
#include <QLayout>
#include <QPushButton>

/* Internal */
#include "Definition.h"
#include "VoidConformer/ConformOptions.h"

VOID_NAMESPACE_OPEN

class ConformOptionsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ConformOptionsDialog(QWidget* parent = nullptr);
    ~ConformOptionsDialog();

    QSize sizeHint() const { return QSize(450, 200); }

    ConformOptions Options() const;

private:
    QVBoxLayout* m_Layout;
    QPushButton* m_AcceptButton;
    QPushButton* m_RejectButton;

    QCheckBox* m_TcMatchCheckBox;
    QCheckBox* m_ReconformCheckBox;

private:
    void Build();
    void Setup();
};

VOID_NAMESPACE_CLOSE

#endif // _CONFORM_OPTIONS_DIALOG_H
