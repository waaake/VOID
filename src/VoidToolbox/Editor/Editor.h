// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _BASE_EDITOR_H
#define _BASE_EDITOR_H

/* Qt */
#include <QLayout>
#include <QLineEdit>
#include <QToolButton>
#include <QWidget>

/* Internal */
#include "Definition.h"
#include "VoidQExtensions/PushButton.h"

VOID_NAMESPACE_OPEN

struct EditorControl
{
    QString name;
    QString label;
    QString description;
    QWidget* widget;

    EditorControl(const QString& name, QWidget* w) : name(name), widget(w) {}
    EditorControl(const QString& name, const QString& label, QWidget* w)
        : name(name), label(label), widget(w) {}
    EditorControl(const QString& name, const QString& label, const QString& description, QWidget* w)
        : name(name), label(label), description(description), widget(w) {}
};

class BaseEditor : public QWidget
{
    Q_OBJECT
protected:
    BaseEditor(QWidget* parent = nullptr);
public:
    virtual ~BaseEditor() = default;

    void Close();

signals:
    void nameChangeRequested(const QString&);
    void colorChangeRequested(const QColor&);
    void stateChangeRequested(bool enabled);

protected:
    void paintEvent(QPaintEvent* event) override;

protected:
    void SetName(const QString& name, bool readonly = false);
    void SetColor(const QColor& color);
    void Enable(bool enabled);
    void AddLabel(const QString& label);
    void AddControl(const EditorControl& editor, bool newline = true);
    void AddControl(const EditorControl& editor, int rspan, int cspan, bool newline = true);
    void AddWidget(const EditorControl& editor, bool newline = true);
    void AddWidget(const EditorControl& editor, int rspan, int cspan, bool newline = true);
    void BeginSection(const QString& name);
    void EndSection() { AddSeparator(); }
    void AddSeparator();
    QWidget* Control(const QString& name);
    virtual void Refresh() {}

private:
    QVBoxLayout* m_Layout;
    QGridLayout* m_Grid;
    QLineEdit* m_NameEdit;
    ColorSelectionButton* m_ColorButton;
    QToolButton* m_EnableButton;
    QToolButton* m_RefreshButton;
    CloseButton* m_CloseButton;

    int m_Row;
    int m_Column;

private:
    void Build();
    void Setup();
};

VOID_NAMESPACE_CLOSE

#endif // _BASE_EDITOR_H
