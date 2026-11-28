// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _COLOR_CONFIG_EDITOR_H
#define _COLOR_CONFIG_EDITOR_H

/* Qt */
#include <QComboBox>
#include <QCheckBox>

/* Internal */
#include "Definition.h"
#include "Editor.h"
#include "Controls/Field.h"

VOID_NAMESPACE_OPEN

class ColorEngine;

class ColorConfigEditor : public BaseEditor
{
    Q_OBJECT
public:
    explicit ColorConfigEditor(ColorEngine* engine, QWidget* parent = nullptr);
    ~ColorConfigEditor();

    bool HasSameEngine(const ColorEngine* engine) const { return m_Engine == engine; }

protected:
    void Refresh() override;

private:
    QComboBox* m_WorkingSpaceCombo;
    QComboBox* m_DisplayCombo;
    QComboBox* m_ViewCombo;
    QCheckBox* m_InverseCheckBox;
    
    ColorEngine* m_Engine;

private:
    void Build();
    void Setup();
};

VOID_NAMESPACE_CLOSE

#endif // _COLOR_CONFIG_EDITOR_H
