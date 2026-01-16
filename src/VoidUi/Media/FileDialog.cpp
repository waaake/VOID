// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QHeaderView>
#include <QKeyEvent>

/* Internal */
#include "FileDialog.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

MediaFileDialog::MediaFileDialog(QWidget* parent)
    : QDialog(parent)
    , m_FileMode(FileMode::ExistingFile)
{
    Build();
    Connect();
    Setup();
}

MediaFileDialog::~MediaFileDialog()
{
    m_OptionsLayout->deleteLater();
    delete m_OptionsLayout;
    m_OptionsLayout = nullptr;

    m_BrowserSplitter->deleteLater();
    delete m_BrowserSplitter;
    m_BrowserSplitter = nullptr;

    m_FileLayout->deleteLater();
    delete m_FileLayout;
    m_FileLayout = nullptr;

    m_FilterLayout->deleteLater();
    delete m_FilterLayout;
    m_FilterLayout = nullptr;

    m_Layout->deleteLater();
    delete m_Layout;
    m_Layout = nullptr;

    m_Model->deleteLater();
    delete m_Model;
    m_Model = nullptr;
}

void MediaFileDialog::SetFileMode(const MediaFileDialog::FileMode& mode)
{
    m_FileMode = mode;
    m_FileMode == FileMode::ExistingFile ? setWindowTitle("Open") : setWindowTitle("Select Directory");
}

void MediaFileDialog::Build()
{
    m_Model = new QFileSystemModel(this);
    m_Model->setRootPath(QDir::rootPath());
    m_Model->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);

    m_Layout = new QVBoxLayout(this);

    m_OptionsLayout = new QHBoxLayout;
    m_FileLayout = new QHBoxLayout;
    m_FilterLayout = new QHBoxLayout;

    m_LookInLabel = new QLabel("Look in:", this);
    m_DirectoryEdit = new QLineEdit(this);

    m_BackButton = new QToolButton(this);
    m_BackButton->setAutoRaise(true);
    m_BackButton->setIcon(style()->standardIcon(style()->SP_ArrowBack));

    m_ForwardButton = new QToolButton(this);
    m_ForwardButton->setAutoRaise(true);
    m_ForwardButton->setIcon(style()->standardIcon(style()->SP_ArrowForward));

    m_UpButton = new QToolButton(this);
    m_UpButton->setAutoRaise(true);
    m_UpButton->setIcon(style()->standardIcon(style()->SP_ArrowUp));

    m_NewDirButton = new QToolButton(this);
    m_NewDirButton->setAutoRaise(true);
    m_NewDirButton->setIcon(style()->standardIcon(style()->SP_FileDialogNewFolder));

    m_BrowserSplitter = new QSplitter(this);

    m_QuickLinks = new QuickLinkView(this);
    m_FileTree = new FileTree(m_Model, this);

    m_BrowserSplitter->addWidget(m_QuickLinks);
    m_BrowserSplitter->addWidget(m_FileTree);

    m_SequencesCheck = new QCheckBox("Sequences");
    m_NameLabel = new QLabel("File name:", this);
    m_FilterLabel = new QLabel("Files of type:", this);

    m_NameEdit = new QLineEdit(this);
    m_FilterCombo = new QComboBox(this);

    m_AcceptButton = new QPushButton();
    m_CancelButton = new QPushButton("Cancel");
    m_CancelButton->setIcon(style()->standardIcon(style()->SP_DialogCancelButton));

    m_OptionsLayout->addWidget(m_LookInLabel);
    m_OptionsLayout->addWidget(m_DirectoryEdit);
    m_OptionsLayout->addWidget(m_BackButton);
    m_OptionsLayout->addWidget(m_ForwardButton);
    m_OptionsLayout->addWidget(m_UpButton);
    m_OptionsLayout->addWidget(m_NewDirButton);

    m_FileLayout->addWidget(m_NameLabel);
    m_FileLayout->addWidget(m_NameEdit);
    m_FileLayout->addWidget(m_AcceptButton);

    m_FilterLayout->addWidget(m_SequencesCheck);
    m_FilterLayout->addWidget(m_FilterLabel);
    m_FilterLayout->addWidget(m_FilterCombo);
    m_FilterLayout->addWidget(m_CancelButton);

    m_Layout->addLayout(m_OptionsLayout);
    m_Layout->addWidget(m_BrowserSplitter);
    m_Layout->addLayout(m_FileLayout);
    m_Layout->addLayout(m_FilterLayout);
}

void MediaFileDialog::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
        m_FileTree->Open();
    else if (event->modifiers() & Qt::AltModifier && event->key() == Qt::Key_Up)
        m_FileTree->CdUp();

    QDialog::keyPressEvent(event);
}

void MediaFileDialog::Connect()
{
    connect(m_DirectoryEdit, &QLineEdit::returnPressed, this, [this]() -> void
    {
        m_FileTree->SetDirectory(m_DirectoryEdit->text());
    });

    connect(m_FileTree, &FileTree::directoryChanged, m_DirectoryEdit, &QLineEdit::setText);
    connect(m_FileTree, &FileTree::highlighted, m_NameEdit, &QLineEdit::setText);
    connect(m_FileTree, &FileTree::accepted, this, &MediaFileDialog::Accept);

    connect(m_QuickLinks, &QuickLinkView::linkSelected, this, [this](const QString& path) -> void
    {
        m_DirectoryEdit->setText(path);
        m_FileTree->SetDirectory(path);
    });

    connect(m_UpButton, &QToolButton::clicked, m_FileTree, &FileTree::CdUp);
    connect(m_AcceptButton, &QPushButton::clicked, m_FileTree, [this]() -> void
    {
        m_FileMode == FileMode::Directory ? m_FileTree->SelectDirectory() : m_FileTree->Open();
    });

    connect(m_CancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_SequencesCheck, &QCheckBox::toggled, m_FileTree, &FileTree::EnableSequences);
}

void MediaFileDialog::Setup()
{
    m_BackButton->setFixedWidth(24);
    m_ForwardButton->setFixedWidth(24);
    m_UpButton->setFixedWidth(24);
    m_NewDirButton->setFixedWidth(24);

    m_AcceptButton->setFixedWidth(90);
    m_CancelButton->setFixedWidth(90);

    m_BrowserSplitter->setSizes({80, 400});
    m_FilterCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    m_BackButton->setEnabled(false);
    m_ForwardButton->setEnabled(false);
    m_SequencesCheck->setChecked(true);    
    
    m_AcceptButton->setText("Open");
    m_AcceptButton->setIcon(style()->standardIcon(style()->SP_DialogOpenButton));
    m_DirectoryEdit->setText(m_FileTree->CurrentDirectory());

    setFocusProxy(m_NameEdit);
    setTabOrder(m_NameEdit, m_DirectoryEdit);
}

void MediaFileDialog::Accept(const QString& selected)
{
    m_SelectedPath = std::move(selected);
    accept();
}

VOID_NAMESPACE_CLOSE
