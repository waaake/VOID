// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _PARAM_EDITOR_H
#define _PARAM_EDITOR_H

/* Qt */
#include <QLayout>
#include <QSlider>
#include <QLineEdit>
#include <QWidget>

/* Internal */
#include "Definition.h"
#include "Param.h"

VOID_NAMESPACE_OPEN

class ParamEditor : public QWidget
{
public:
    ParamEditor(Param* param, QWidget* parent = nullptr);
    virtual ~ParamEditor();

protected:
    virtual QWidget* Widget() = 0;

protected:
    Param* m_Param;

private: /* Members */
    QHBoxLayout* m_Layout;

private: /* Methods */
    void Build();
};

VOID_NAMESPACE_CLOSE

#endif // _PARAM_EDITOR_H
