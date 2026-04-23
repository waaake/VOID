// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _PROPERTIES_EDITOR_H
#define _PROPERTIES_EDITOR_H

/* Qt */
#include <QWidget>
#include <QLayout>
#include <QPushButton>

/* Internal */
#include "Definition.h"
#include "Effects.h"
#include "VoidUi/QExtensions/LineEdit.h"

VOID_NAMESPACE_OPEN

class PropertiesPanel : public QWidget
{
    Q_OBJECT

public:
    explicit PropertiesPanel(QWidget* parent = nullptr);
    ~PropertiesPanel();

    QSize sizeHint() const override { return QSize(300, 720); }

    void Show(QWidget* panel);
    void EditEffect(Effect* effect);

    void Clear();
    void ClearAdditionalPanels(int limit);

private: /* Members */
    QVBoxLayout* m_Layout;
    QVBoxLayout* m_ScrollLayout;
    QPushButton* m_ClearButton;
    IntBoundLineEdit* m_PanelCounter;

private: /* Methods */
    void Build();
    void Setup();
    void Connect();
};

VOID_NAMESPACE_CLOSE

#endif // _PROPERTIES_EDITOR_H
