// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_Q_EXT_PROGRESS_TASK_H
#define _VOID_Q_EXT_PROGRESS_TASK_H

/* Qt */
#include <QDialog>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QProgressBar>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class ProgressTask : public QDialog
{
    Q_OBJECT

public:
    ProgressTask(QWidget* parent = nullptr);
    ~ProgressTask();

    inline QSize sizeHint() const { return QSize(500, 40); }

    inline bool Cancelled() const { return m_Cancelled; }
    void Cancel();

    inline void SetTaskType(const char* text) { m_TaskTypeLabel->setText(text); }
    inline void SetCurrentTask(const char* text) { SetLabelTask(text); }

    inline void SetMaximum(int max) { m_ProgressBar->setMaximum(max); }
    inline void SetValue(int value) { m_ProgressBar->setValue(value); }
    inline int Value() const { return m_ProgressBar->value(); }

signals:
    void cancelled();

protected: /* Members */
    bool m_Cancelled;

private: /* Members */
    QVBoxLayout* m_Layout;
    QHBoxLayout* m_TaskLayout;
    QHBoxLayout* m_ButtonLayout;

    QPushButton* m_CancelButton;
    QLabel* m_TaskTypeLabel;
    QLabel* m_CurrentTaskLabel;

    QProgressBar* m_ProgressBar;

private: /* Methods */
    void Build();
    void Setup();
    void Connect();

    /**
     * The curren task label could be of any width, and we don't necessarily want to expand to those widths
     * and the next task could be lesser in width making the UI too inconsistent all the time, hence a fixed
     * label width is set, if the width goes beyond that, it gets elided from the left side
     */
    void SetLabelTask(const char* text);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_Q_EXT_PROGRESS_TASK_H
