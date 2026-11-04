// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "ParamEditor.h"

VOID_NAMESPACE_OPEN

ParamEditor::ParamEditor(Param* param, QWidget* parent)
    : QWidget(parent)
{
    Build();
}

ParamEditor::~ParamEditor()
{
    m_Layout->deleteLater();
    delete m_Layout;
    m_Layout = nullptr;
}

void ParamEditor::Build()
{
    m_Layout = new QHBoxLayout(this);
    m_Layout->setContentsMargins(0, 0, 0, 0);
    m_Layout->addWidget(Widget());
}

VOID_NAMESPACE_CLOSE
