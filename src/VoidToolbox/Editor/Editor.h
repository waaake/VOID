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
    void SetName(const QString& name);
    void SetColor(const QColor& color);
    void Enable(bool enabled);
    void AddControl(const QString& label, QWidget* editor, bool newline = true);
    void AddControl(const QString& label, QWidget* editor, int rspan, int cspan, bool newline = true);
    void AddWidget(QWidget* editor, bool newline = true);
    void AddWidget(QWidget* editor, int rspan, int cspan, bool newline = true);
    void AddSeparator();

private:
    QVBoxLayout* m_Layout;
    QGridLayout* m_Grid;
    QLineEdit* m_NameEdit;
    ColorSelectionButton* m_ColorButton;
    QToolButton* m_EnableButton;
    CloseButton* m_CloseButton;

    int m_Row;
    int m_Column;

private:
    void Build();
    void Setup();
};

VOID_NAMESPACE_CLOSE

#endif // _BASE_EDITOR_H
