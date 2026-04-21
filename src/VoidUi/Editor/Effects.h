// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _EFFECTS_EDITOR_H
#define _EFFECTS_EDITOR_H

/* Qt */
#include <QWidget>
#include <QLayout>
#include <QLineEdit>
#include <QToolButton>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Effects/Effects.h"
#include "VoidUi/QExtensions/PushButton.h"

VOID_NAMESPACE_OPEN

class EffectEditor : public QWidget
{
    Q_OBJECT

public:
    EffectEditor(Effect* effect, QWidget* parent = nullptr);
    ~EffectEditor();

    void Close();

protected:
    void paintEvent(QPaintEvent* event) override;

private: /* Members */
    QVBoxLayout* m_Layout;
    QLineEdit* m_NameEdit;
    QToolButton* m_EnableButton;
    CloseButton* m_CloseButton;

    Effect* m_Effect;

private: /* Methods */
    void Build();
    void Setup();
    void Connect();
};

VOID_NAMESPACE_CLOSE

#endif // _EFFECTS_EDITOR_H
