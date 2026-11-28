// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _COLOR_ENGINE_H
#define _COLOR_ENGINE_H

/* Qt */
#include <QObject>
#include <QStringList>

/* Internal */
#include "Definition.h"
#include "VoidCore/Processors/ColorProcessor.h"

VOID_NAMESPACE_OPEN

class VOID_API ColorEngine : public QObject
{
    Q_OBJECT
public:
    explicit ColorEngine(QObject* parent = nullptr);
    void Init();

    QStringList Roles() const;
    QStringList Views() const;
    QStringList Displays() const;

    QString WorkingSpace() const;
    QString ActiveView() const { return m_View; };
    QString ActiveDisplay() const { return m_Display; }

    void SetDisplay(const QString& display);
    void SetView(const QString& view);

signals:
    void displayChanged();
    void viewChanged();
    void updated();

private:
    QString m_View;
    QString m_Display;
    ColorProcessor& m_Processor;
};

VOID_NAMESPACE_CLOSE

#endif // _COLOR_ENGINE_H
