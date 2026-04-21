// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _PROPERTIES_EDITOR_H
#define _PROPERTIES_EDITOR_H

/* Qt */
#include <QWidget>
#include <QLayout>

/* Internal */
#include "Definition.h"
#include "Effects.h"

VOID_NAMESPACE_OPEN

class PropertiesPanel : public QWidget
{
    Q_OBJECT

public:
    explicit PropertiesPanel(QWidget* parent = nullptr);
    ~PropertiesPanel();

    void Show(QWidget* panel);
    void EditEffect(Effect* effect);

private: /* Members */
    QVBoxLayout* m_Layout;
    QVBoxLayout* m_ScrollLayout;

private: /* Methods */
    void Build();
};

VOID_NAMESPACE_CLOSE

#endif // _PROPERTIES_EDITOR_H
