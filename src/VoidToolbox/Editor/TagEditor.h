// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _TAG_EDITOR_H
#define _TAG_EDITOR_H

/* Qt */
#include <QLayout>
#include <QLineEdit>
#include <QListView>
#include <QPushButton>
#include <QTreeView>

/* Internal */
#include "Definition.h"
#include "VoidQExtensions/Dialog.h"

VOID_NAMESPACE_OPEN

class Tag;

class VOID_API TagBase : public QWidget
{
public:
    TagBase(QWidget* parent = nullptr);
    ~TagBase();
    
    QString Name() const { return m_NameEdit->text(); }
    void SetName(const QString& name) const { m_NameEdit->setText(name); }
    void Clear();

    void SetModel(QAbstractItemModel* model) { m_DataTree->setModel(model); }
    void ResetFocus() { m_NameEdit->setFocus(); }

private: /* Members */
    QGridLayout* m_Layout;
    QLineEdit* m_NameEdit;
    QTreeView* m_DataTree;

private: /* Methods */
    void Build();
    void Setup();
};

class VOID_API TagWidget : public TranslucentDialog
{
public:
    TagWidget(QWidget* parent = nullptr);
    ~TagWidget();

    void MoveTo(const QPoint& position);

protected:
    void showEvent(QShowEvent* event) override;

protected:
    QVBoxLayout* m_Layout;
    TagBase* m_TagBase;
    QPushButton* m_AcceptButton;

private:
    void Build();
};

class VOID_API TagEditor : public TranslucentDialog
{
public:
    TagEditor(QWidget* parent = nullptr);
    ~TagEditor();

    void MoveTo(const QPoint& position);

protected:
    inline void SetModel(QAbstractItemModel* model) { m_TagList->setModel(model); }
    inline QModelIndex CurrentTagIndex() const { return m_TagList->currentIndex(); }
    void SetCurrentTag(const QModelIndex& index);
    void SetCurrentTag(const Tag* tag);
    void ResetTag();

protected:
    QVBoxLayout* m_Layout;
    QListView* m_TagList;
    TagBase* m_TagBase;
    QPushButton* m_RemoveButton;

private:
    void Build();
};

VOID_NAMESPACE_CLOSE

#endif // _TAG_EDITOR_H
