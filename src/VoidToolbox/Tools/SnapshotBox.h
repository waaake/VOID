// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SNAPSHOT_BOX_H
#define _SNAPSHOT_BOX_H

/* Qt */
#include <QDialog>
#include <QTreeView>
#include <QPushButton>
#include <QTextEdit>

/* Internal */
#include "Definition.h"
#include "VoidToolbox/Tools/Models/SnapshotModel.h"

VOID_NAMESPACE_OPEN

class SnapshotBox : public QDialog
{
public:
    SnapshotBox(QWidget* parent = nullptr);
    ~SnapshotBox();

    QSize sizeHint() const override { return QSize(340, 180); }

    std::string Name() const { return m_Name; }
    std::string Description() const { return m_DescriptionBox->toPlainText().toStdString(); }

private:
    QPushButton* m_SaveButton;
    QPushButton* m_CancelButton;
    QTextEdit* m_DescriptionBox;

    std::string m_Name;

private:
    void Build();
    void Connect();
};

class RestoreSnapshotBox : public QDialog
{
public:
    RestoreSnapshotBox(std::vector<Snapshot>& snapshots, QWidget* parent = nullptr);
    ~RestoreSnapshotBox();

    QSize sizeHint() const override { return QSize(600, 400); }

    int Index() const { return m_View->currentIndex().row(); }

private:
    QPushButton* m_SaveButton;
    QPushButton* m_CancelButton;
    QTreeView* m_View;
    SnapshotModel* m_Model;

private:
    void Build();
    void Connect();
    void Setup();
};

VOID_NAMESPACE_CLOSE

#endif // _SNAPSHOT_BOX_H
