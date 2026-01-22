// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_FILE_DIALOG_H
#define _VOID_FILE_DIALOG_H

/* Qt */
#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QSplitter>

/* Internal */
#include "Definition.h"
#include "VoidUi/Media/Views/FileView.h"

VOID_NAMESPACE_OPEN

class MediaFileDialog : public QDialog
{
    Q_OBJECT

public: /* Enums */
    enum class FileMode
    {
        ExistingFile,
        Directory
    };

public:
    MediaFileDialog(QWidget* parent = nullptr);
    ~MediaFileDialog();

    inline QSize sizeHint() const override { return QSize(800, 400); }

    /**
     * @brief Set the File Mode for the dialog.
     * 
     * @param mode FileMode enum to set (ExistingFile | Directory).
     */
    void SetFileMode(const FileMode& mode);

    /**
     * @brief Returns the currently selected path, in case the Dialog was accepted.
     * 
     * @return QString Current selected path.
     */
    inline QString SelectedPath() const { return m_SelectedPath; }

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void showEvent(QShowEvent* event) override;

private: /* Members */
    QPushButton* m_AcceptButton;
    QPushButton* m_CancelButton;
    QToolButton* m_BackButton;
    QToolButton* m_ForwardButton;
    QToolButton* m_UpButton;
    QToolButton* m_NewDirButton;
    QuickLinkView* m_QuickLinks;
    FileTree* m_FileTree;
    
    QLabel* m_LookInLabel;
    QLabel* m_NameLabel;
    QLabel* m_FilterLabel;
    QLineEdit* m_NameEdit;
    QLineEdit* m_DirectoryEdit;
    QComboBox* m_FilterCombo;
    QCheckBox* m_SequencesCheck;

    QVBoxLayout* m_Layout;
    QHBoxLayout* m_OptionsLayout;
    QHBoxLayout* m_FileLayout;
    QHBoxLayout* m_FilterLayout;
    QSplitter* m_BrowserSplitter;
    QFileSystemModel* m_Model;

    QString m_SelectedPath;
    FileMode m_FileMode;

private: /* Methods */
    void Build();
    void Connect();
    void Setup();

    void Accept(const QString& selected);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_FILE_DIALOG_H
