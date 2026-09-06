// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>

/* Internal */
#include "SnapshotBox.h"
#include "VoidCore/VoidTools.h"
#include "VoidQExtensions/Delegates.h"

VOID_NAMESPACE_OPEN

SnapshotBox::SnapshotBox(QWidget* parent)
    : QDialog(parent)
    , m_Name(Tools::timestamp("snapshot"))
{
    setWindowTitle("Add Snapshot");

    Build();
    Connect();
}

SnapshotBox::~SnapshotBox()
{
    m_DescriptionBox->deleteLater();
    delete m_DescriptionBox;
    m_DescriptionBox = nullptr;

    m_SaveButton->deleteLater();
    delete m_SaveButton;
    m_SaveButton = nullptr;

    m_CancelButton->deleteLater();
    delete m_CancelButton;
    m_CancelButton = nullptr;
}

void SnapshotBox::Build()
{
    QGridLayout* layout = new QGridLayout(this);
    m_DescriptionBox = new QTextEdit;

    m_SaveButton = new QPushButton("Add");
    m_CancelButton = new QPushButton("Cancel");


    QLabel* name = new QLabel("Name:", this);
    QLabel* description = new QLabel("Description:", this);
    QFont f = name->font();
    f.setBold(true);

    name->setFont(f);
    description->setFont(f);

    layout->addWidget(name, 0, 0);
    layout->addWidget(new QLabel(m_Name.c_str(), this), 0, 1, 1, 3);
    layout->addWidget(description, 1, 0);
    layout->addWidget(m_DescriptionBox, 1, 1, 4, 4);

    layout->addWidget(m_SaveButton, 5, 3, 1, 1);
    layout->addWidget(m_CancelButton, 5, 4, 1, 1);
}

void SnapshotBox::Connect()
{
    connect(m_SaveButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_CancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

/// RestoreSnapshotBox

RestoreSnapshotBox::RestoreSnapshotBox(std::vector<Snapshot>& snapshots, QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Restore from Snapshot");

    m_Model = new SnapshotModel(snapshots, this);
    Build();
    Connect();
    Setup();
}

RestoreSnapshotBox::~RestoreSnapshotBox()
{
    m_Model->deleteLater();
    delete m_Model;
    m_Model = nullptr;

    m_View->deleteLater();
    delete m_View;
    m_View = nullptr;

    m_SaveButton->deleteLater();
    delete m_SaveButton;
    m_SaveButton = nullptr;

    m_CancelButton->deleteLater();
    delete m_CancelButton;
    m_CancelButton = nullptr;
}

void RestoreSnapshotBox::Build()
{
    QGridLayout* layout = new QGridLayout(this);
    m_View = new QTreeView;

    m_View->setModel(m_Model);

    m_SaveButton = new QPushButton("Restore");
    m_CancelButton = new QPushButton("Cancel");

    QLabel* name = new QLabel("Select a Snapshot to restore from:", this);
    QFont f = name->font();
    f.setBold(true);

    name->setFont(f);

    layout->addWidget(name, 0, 0, 1, 3);
    layout->addWidget(m_View, 1, 0, 4, 6);

    layout->addWidget(m_SaveButton, 5, 4, 1, 1);
    layout->addWidget(m_CancelButton, 5, 5, 1, 1);

    layout->setRowStretch(1, 1);
}

void RestoreSnapshotBox::Connect()
{
    connect(m_SaveButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_CancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void RestoreSnapshotBox::Setup()
{
    QHeaderView* h = m_View->header();
    h->setSectionResizeMode(0, QHeaderView::Interactive);
    h->setSectionResizeMode(1, QHeaderView::Stretch);

    // We just need to cover snapshot_20260512_022445 as the name width initially
    m_View->setColumnWidth(0, 220);
    m_View->setAlternatingRowColors(true);
    m_View->setFocusPolicy(Qt::FocusPolicy::ClickFocus);

    m_View->setItemDelegate(new HCustomItemDelegate(40, m_View));
}

VOID_NAMESPACE_CLOSE
