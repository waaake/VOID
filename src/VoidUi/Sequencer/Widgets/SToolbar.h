// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_TOOLBAR_H
#define _SEQUENCER_TOOLBAR_H

/* Qt */
#include <QFrame>
#include <QLayout>
#include <QToolButton>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class SToolbar : public QFrame
{
    Q_OBJECT
public:
    explicit SToolbar(QWidget* parent = nullptr);

signals:
    void reset();

private:
    QVBoxLayout* m_Layout;
    QToolButton* m_ResetButton;

private: /* Methods */
    void Build();
    void Connect();
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_TOOLBAR_H
