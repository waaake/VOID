// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QGridLayout>
#include <QLabel>
#include <QVariant>

/* Internal */
#include "ConformOptions.h"
#include "FormatForge.h"

VOID_NAMESPACE_OPEN

/// RuleCheckBox

class RuleCheckbox : public QCheckBox
{
public:
    RuleCheckbox(ConformRule* rule, QWidget* parent = nullptr)
        : QCheckBox(rule->Name().c_str(), parent), rule(rule) {}

    ConformRule* Rule() { return rule; }

private:
    ConformRule* rule;
};


/// ConformOptionsDialog

ConformOptionsDialog::ConformOptionsDialog(QWidget* parent)
    : QDialog(parent)
{
    Build();
    Setup();
}

ConformOptionsDialog::~ConformOptionsDialog()
{
    m_Layout->deleteLater();
    delete m_Layout;
    m_Layout = nullptr;   
}

ConformOptions ConformOptionsDialog::Options() const
{
    ConformRules rules;
    const QList<RuleCheckbox*> cbs = findChildren<RuleCheckbox*>();
    rules.reserve(cbs.size());

    for (const auto& cb : cbs)
    {
        /// For some reason, the findChildren call above yields qcheckboxes as well
        // making the TcMatchCheckBox and ReconfomCheckBox as a valid entity here
        // this causes the rules to push a rule in an invalid state which can't be detected safely
        // and leads to a crash -- for now keeping this here, need to investigate this behaviour in qt6
        if (cb == m_TcMatchCheckBox || cb == m_ReconformCheckBox)
            continue;

        if (cb->isChecked())
            rules.push_back(cb->Rule());
    }

    return ConformOptions(rules, m_TcMatchCheckBox->isChecked(), m_ReconformCheckBox->isChecked());
}

void ConformOptionsDialog::Build()
{
    m_Layout = new QVBoxLayout(this);

    QGridLayout* rulegrid = new QGridLayout;

    const int columns = 3;
    const ConformRules rules = Forge::Instance().ConformRules();
    int row = 0;
    for (int i = 0; i < static_cast<int>(rules.size()); ++i)
    {
        RuleCheckbox* cb = new RuleCheckbox(rules[i], this);
        cb->setChecked(true);

        row = i / columns;
        rulegrid->addWidget(cb, row, i % columns);
    }
    rulegrid->setRowStretch(++row, 1);

    m_TcMatchCheckBox = new QCheckBox("Accept best timecode match if no rules match.");
    m_ReconformCheckBox = new QCheckBox("Conform shots that already have media.");

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    m_AcceptButton = new QPushButton("Ok");
    m_RejectButton = new QPushButton("Cancel");

    buttonLayout->addStretch(1);
    buttonLayout->addWidget(m_AcceptButton);
    buttonLayout->addWidget(m_RejectButton);

    QLabel* rlabel = new QLabel("Conform Rules:", this);
    QLabel* addlabel = new QLabel("Additional Options:", this);
    QFont f(rlabel->font());
    f.setBold(true);
    rlabel->setFont(f);
    addlabel->setFont(f);

    m_Layout->addWidget(rlabel);
    m_Layout->addLayout(rulegrid);

    m_Layout->addWidget(addlabel);
    m_Layout->addWidget(m_TcMatchCheckBox);
    m_Layout->addWidget(m_ReconformCheckBox);

    m_Layout->addLayout(buttonLayout);
}

void ConformOptionsDialog::Setup()
{
    setWindowTitle("Conform Options");

    connect(m_AcceptButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_RejectButton, &QPushButton::clicked, this, &QDialog::reject);
}

VOID_NAMESPACE_CLOSE
