// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_ABOUT_H
#define _VOID_ABOUT_H

/* Qt */
#include <QDialog>
#include <QFrame>
#include <QLabel>
#include <QLayout>
#include <QPushButton>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class AboutVoid : public QDialog
{
    Q_OBJECT

public:
    AboutVoid(QWidget* parent = nullptr);
    virtual ~AboutVoid();

private: /* Members */
    QPushButton* m_OkButton;

    /* Main Layout for the Dialog */
    QVBoxLayout* m_Layout;
    QHBoxLayout* m_ButtonLayout;

    /* Internal Layout for Details */
    QGridLayout* m_DetailsLayout;

    /* Separators */
    QFrame* m_LogoSeparator;
    QFrame* m_ButtonSeparator;

    /* Logo */
    QLabel* m_VoidLabel;

    /* Description */
    QLabel* m_About;
    QLabel* m_Description;

    /* Details */
    QLabel* m_AuthorHeader;
    QLabel* m_Author;

    QLabel* m_ContactHeader;
    QLabel* m_Contact;
    
    QLabel* m_VersionHeader;
    QLabel* m_Version;

    QLabel* m_GithubHeader;
    QLabel* m_Github;

private: /* Methods */
    void Build();
    void Connect();

};

VOID_NAMESPACE_CLOSE

#endif // _VOID_ABOUT_H
