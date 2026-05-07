// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QFont>
#include <QPixmap>

/* Internal */
#include "About.h"

static const char* DESCRIPTION = \
"VOID is a media player with evolving capabilities.\n\
Currently functional as a playback tool, it is on track to \
becoming a fully-featured review system for managing media.";

static const char* AUTHOR = "Vivek Verma";
static const char* CONTACT = "vivek_ve@outlook.com";
static const char* GITHUB = "github.com/waaake/VOID";

VOID_NAMESPACE_OPEN

AboutVoid::AboutVoid(QWidget* parent)
    : QDialog(parent)
{
    Build();
    connect(m_OkButton, &QPushButton::clicked, this, &QDialog::close);

    setFixedSize(600, 400);
    setWindowFlag(Qt::FramelessWindowHint);
}

AboutVoid::~AboutVoid()
{
    /* Delete the main layout at last*/
    m_Layout->deleteLater();
    delete m_Layout;
}

void AboutVoid::Build()
{
    /* Base Layout */
    m_Layout = new QVBoxLayout(this);

    /* VOID Logo */
    m_VoidLabel = new QLabel;
    QImage logo(":resources/images/VOID_Logo_900x200.png");
    m_VoidLabel->setPixmap(QPixmap::fromImage(logo.scaledToWidth(400, Qt::SmoothTransformation)));

    /* --------------------------------------------- */
    m_LogoSeparator = new QFrame;
    m_LogoSeparator->setFrameShape(QFrame::HLine);
    m_LogoSeparator->setFrameShadow(QFrame::Raised);
    m_LogoSeparator->setLineWidth(2);
    m_LogoSeparator->setMidLineWidth(3);

    /* Description */
    m_About = new QLabel("About VOID");

    QFont f;
    f.setPixelSize(14);
    f.setBold(true);

    m_About->setFont(f);

    m_Description = new QLabel(DESCRIPTION);
    m_Description->setWordWrap(true);
    m_Description->setMaximumWidth(600);

    /* Details */
    /* Font for header labels */
    QFont header;
    header.setPixelSize(12);
    header.setBold(true);

    /* Details */
    m_AuthorHeader = new QLabel("Author: ");
    m_AuthorHeader->setFont(header);
    m_Author = new QLabel(AUTHOR);

    /**
     * Set Stretch on the label
     * Setting this even on only one text makes the entire grid follow 
     */
    m_Author->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    m_ContactHeader = new QLabel("Contact: ");
    m_ContactHeader->setFont(header);
    // m_Contact = new QLabel(CONTACT, this);
    
    m_VersionHeader = new QLabel("Version: ");
    m_VersionHeader->setFont(header);
    // m_Version = new QLabel(VOID_VERSION_STRING, this);

    m_GithubHeader = new QLabel("Github: ");
    m_GithubHeader->setFont(header);
    // m_Github = new QLabel(GITHUB);

    /* Add to details layouts */
    m_DetailsLayout = new QGridLayout;

    m_DetailsLayout->addWidget(m_AuthorHeader, 0, 0, Qt::AlignLeft);
    m_DetailsLayout->addWidget(m_Author, 0, 1);

    m_DetailsLayout->addWidget(m_ContactHeader, 1, 0, Qt::AlignLeft);
    m_DetailsLayout->addWidget(new QLabel(CONTACT, this), 1, 1);

    m_DetailsLayout->addWidget(m_VersionHeader, 2, 0, Qt::AlignLeft);
    m_DetailsLayout->addWidget(new QLabel(VOID_VERSION_STRING, this), 2, 1);

    m_DetailsLayout->addWidget(m_GithubHeader, 3, 0, Qt::AlignLeft);
    m_DetailsLayout->addWidget(new QLabel(GITHUB, this), 3, 1);

    /* --------------------------------------------- */
    m_ButtonSeparator = new QFrame;
    m_ButtonSeparator->setFrameShape(QFrame::HLine);
    m_ButtonSeparator->setFrameShadow(QFrame::Raised);
    m_ButtonSeparator->setLineWidth(2);
    m_ButtonSeparator->setMidLineWidth(3);

    /* Buttons */
    m_ButtonLayout = new QHBoxLayout;
    m_OkButton = new QPushButton("Close");

    m_ButtonLayout->addStretch(1);
    m_ButtonLayout->addWidget(m_OkButton);

    /* Add to the Layout */
    m_Layout->addWidget(m_VoidLabel);
    m_Layout->addWidget(m_LogoSeparator);
    m_Layout->addWidget(m_About);
    m_Layout->addWidget(m_Description);

    /* Spacing */
    m_Layout->addStretch(1);

    m_Layout->addLayout(m_DetailsLayout);

    /* Spacing */
    m_Layout->addStretch(1);
    m_Layout->addWidget(m_ButtonSeparator);
    m_Layout->addLayout(m_ButtonLayout);
}

VOID_NAMESPACE_CLOSE
