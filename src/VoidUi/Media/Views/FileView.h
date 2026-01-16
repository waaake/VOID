// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_FILE_VIEW_H
#define _VOID_FILE_VIEW_H

/* Qt */
#include <QComboBox>
#include <QFileSystemModel>
#include <QListView>
#include <QTableView>
#include <QTreeView>
#include <QListWidget>

/* Internal */
#include "Definition.h"
#include "VoidUi/Media/Models/MediaFilesModel.h"

VOID_NAMESPACE_OPEN

class FileTree : public QTreeView
{
    Q_OBJECT
public:
    FileTree(QFileSystemModel* model, QWidget* parent = nullptr);
    ~FileTree();

    void setRootIndex(const QModelIndex& index) override;

    /**
     * @brief Returns the current root directory from the model.
     * 
     * @return QString The current root directory path.
     */
    QString CurrentDirectory() const;

    /**
     * @brief Set the Directory as the root index of the model.
     * 
     * @param directory Directory to set as the current root.
     */
    void SetDirectory(const QString& directory);

    /**
     * @brief Move upwards in the directory tree.
     * 
     */
    void CdUp();
    /**
     * @brief Looks at the current selected index in the View and tries to open
     * or proceed into the directory depending on what's possible with the index
     * if a directory is selected and this method is called, it will traverse into
     * the selected directory and emits the directoryChanged signal, if however a valid
     * file is selected, it will then emit the accepted signal
     * 
     */
    void Open();
    /**
     * @brief Looks at the provided index in the View and tries to open
     * or proceed into the directory depending on what's possible with the index
     * if a directory is selected and this method is called, it will traverse into
     * the selected directory and emits the directoryChanged signal, if however a valid
     * file is selected, it will then emit the accepted signal
     * 
     * @param index Model index of an entry (directory or file)
     */
    void Open(const QModelIndex& index);

    /**
     * @brief Accepts the current directory and emits the accepted signal with the fullpath
     * to the current or the highlighted directory
     * 
     */
    void SelectDirectory();

    /**
     * @brief Enables display of templated sequence style for Image sequences.
     * 
     * @param enable true to enable sequences.
     * @param disable true to disable sequences and display normal entities in the View.
     */
    inline void EnableSequences(bool enable) { m_Proxy->EnableSequences(enable); }

signals:
    void accepted(const QString&);
    void directoryChanged(const QString&);
    void highlighted(const QString&);

protected:
    void currentChanged(const QModelIndex& current, const QModelIndex& previous) override;

private: /* Members */
    QFileSystemModel* m_Model;
    MediaFilesProxyModel* m_Proxy;

private: /* Methods */
    void Setup();
    void Connect();
    void SetRootIndex(const QModelIndex& index);
};

class QuickLinkView : public QListWidget
{
    Q_OBJECT
public:
    explicit QuickLinkView(QWidget* parent = nullptr);
    ~QuickLinkView();

signals:
    void linkSelected(const QString& path);

private:
    void Setup();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_FILE_VIEW_H
