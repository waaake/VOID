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
    /* Build Base UI */
    Build();

    /* Connect Signals */
    Connect();

    /* Fixed size of the dialog */
    setFixedSize(600, 400);

    /* Frameless */
    setWindowFlag(Qt::FramelessWindowHint);
}

AboutVoid::~AboutVoid()
{
    /* Cleanup the widgets first */
    m_OkButton->deleteLater();
    delete m_OkButton;
    m_LogoSeparator->deleteLater();
    delete m_LogoSeparator;
    m_ButtonSeparator->deleteLater();
    delete m_ButtonSeparator;
    m_VoidLabel->deleteLater();
    delete m_VoidLabel;
    m_About->deleteLater();
    delete m_About;
    m_Description->deleteLater();
    delete m_Description;
    m_AuthorHeader->deleteLater();
    delete m_AuthorHeader;
    m_Author->deleteLater();
    delete m_Author;
    m_ContactHeader->deleteLater();
    delete m_ContactHeader;
    m_Contact->deleteLater();
    delete m_Contact;
    m_VersionHeader->deleteLater();
    delete m_VersionHeader;
    m_Version->deleteLater();
    delete m_Version;
    m_GithubHeader->deleteLater();
    delete m_GithubHeader;
    m_Github->deleteLater();
    delete m_Github;

    /* Delete the layouts after we have deleted the widgets */
    m_ButtonLayout->deleteLater();
    delete m_ButtonLayout;
    m_AuthorLayout->deleteLater();
    delete m_AuthorLayout;
    m_ContactLayout->deleteLater();
    delete m_ContactLayout;
    m_VersionLayout->deleteLater();
    delete m_VersionLayout;
    m_GithubLayout->deleteLater();
    delete m_GithubLayout;
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
    QImage logo("images/VOID_Logo_900x200.png");
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

    m_ContactHeader = new QLabel("Contact: ");
    m_ContactHeader->setFont(header);
    m_Contact = new QLabel(CONTACT);
    
    m_VersionHeader = new QLabel("Version: ");
    m_VersionHeader->setFont(header);
    m_Version = new QLabel("v0.0.1-alpha");

    m_GithubHeader = new QLabel("Github: ");
    m_GithubHeader->setFont(header);
    m_Github = new QLabel(GITHUB);

    /* Add to details layouts */
    m_AuthorLayout = new QHBoxLayout;
    m_ContactLayout = new QHBoxLayout;
    m_VersionLayout = new QHBoxLayout;
    m_GithubLayout = new QHBoxLayout;

    m_AuthorLayout->addWidget(m_AuthorHeader);
    m_AuthorLayout->addWidget(m_Author);
    m_AuthorLayout->addStretch(1);

    m_ContactLayout->addWidget(m_ContactHeader);
    m_ContactLayout->addWidget(m_Contact);
    m_ContactLayout->addStretch(1);

    m_VersionLayout->addWidget(m_VersionHeader);
    m_VersionLayout->addWidget(m_Version);
    m_VersionLayout->addStretch(1);

    m_GithubLayout->addWidget(m_GithubHeader);
    m_GithubLayout->addWidget(m_Github);
    m_GithubLayout->addStretch(1);

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

    m_Layout->addLayout(m_AuthorLayout);
    m_Layout->addLayout(m_ContactLayout);
    m_Layout->addLayout(m_VersionLayout);
    m_Layout->addLayout(m_GithubLayout);

    /* Spacing */
    m_Layout->addStretch(1);
    m_Layout->addWidget(m_ButtonSeparator);
    m_Layout->addLayout(m_ButtonLayout);
}

void AboutVoid::Connect()
{
    connect(m_OkButton, &QPushButton::clicked, this, &QDialog::close);
}

VOID_NAMESPACE_CLOSE
