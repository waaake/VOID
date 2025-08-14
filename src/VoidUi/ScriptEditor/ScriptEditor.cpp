// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QFont>
#include <QFileDialog>
#include <QScrollBar>
#include <QTextStream>

/* Internal */
#include "ScriptEditor.h"
#include "VoidCore/Logging.h"
#include "VoidUi/QExtensions/Tooltip.h"

VOID_NAMESPACE_OPEN

PyScriptEditor::PyScriptEditor(QWidget* parent)
    : QWidget(parent)
{    
    Build();
    Connect();
    Setup();

    m_Executor = new PyExecutor;
    m_Executor->SetOutputCallback([this](const std::string& output)
    {
        QString res = "# ";
        res += QString::fromStdString(output);
        m_OutputConsole->appendPlainText(res);

        m_OutputConsole->verticalScrollBar()->setValue(m_OutputConsole->verticalScrollBar()->maximum());
    });
}

PyScriptEditor::~PyScriptEditor()
{
    delete m_Executor;
    m_Executor = nullptr;

    m_Layout->deleteLater();
    delete m_Layout;
    m_Layout = nullptr;
}

void PyScriptEditor::Build()
{
    /* Base Layout */
    m_Layout = new QVBoxLayout(this);
    m_ButtonLayout = new QHBoxLayout();

    m_ExecAllButton = new QPushButton;
    m_ExecAllButton->setIcon(QIcon(":resources/icons/icon_play.svg"));
    m_ExecAllButton->setToolTip(ToolTipString("Execute", "Execute all code.").c_str());

    m_ExecSelectionButton = new QPushButton;
    m_ExecSelectionButton->setIcon(QIcon(":resources/icons/icon_play_selected.svg"));
    m_ExecSelectionButton->setToolTip(ToolTipString("Execute Selected", "Execute selected code.").c_str());

    m_SaveScriptButton = new QPushButton;
    m_SaveScriptButton->setIcon(QIcon(":resources/icons/icon_save.svg"));
    m_SaveScriptButton->setToolTip(ToolTipString("Save Script", "Saves current Script.").c_str());

    m_LoadScriptButton = new QPushButton;
    m_LoadScriptButton->setIcon(QIcon(":resources/icons/icon_load.svg"));
    m_LoadScriptButton->setToolTip(ToolTipString("Open Script", "Open a python script.").c_str());

    m_ClearOutputButton = new QPushButton;
    m_ClearOutputButton->setIcon(QIcon(":resources/icons/icon_backspace.svg"));
    m_ClearOutputButton->setToolTip(ToolTipString("Clear Output", "Clears the output window.").c_str());

    m_ButtonLayout->addWidget(m_ExecAllButton);
    m_ButtonLayout->addWidget(m_ExecSelectionButton);
    m_ButtonLayout->addSpacing(10);
    m_ButtonLayout->addWidget(m_SaveScriptButton);
    m_ButtonLayout->addWidget(m_LoadScriptButton);
    m_ButtonLayout->addSpacing(10);
    m_ButtonLayout->addWidget(m_ClearOutputButton);
    m_ButtonLayout->addStretch(1);

    /* Consoles */
    m_InternalSplitter = new QSplitter(Qt::Vertical);

    m_OutputConsole = new QPlainTextEdit;
    m_InputConsole = new InputScriptConsole;

    m_InternalSplitter->addWidget(m_OutputConsole);
    m_InternalSplitter->addWidget(m_InputConsole);

    m_Layout->addLayout(m_ButtonLayout);
    m_Layout->addWidget(m_InternalSplitter);
}

void PyScriptEditor::Connect()
{
    connect(m_ExecAllButton, &QPushButton::clicked, this, &PyScriptEditor::ExecuteAll);
    connect(m_InputConsole, &InputScriptConsole::execute, this, &PyScriptEditor::ExecuteAll);

    connect(m_ExecSelectionButton, &QPushButton::clicked, this, &PyScriptEditor::ExecuteSelection);
    connect(m_InputConsole, &InputScriptConsole::executeSelected, this, &PyScriptEditor::ExecuteSelection);

    connect(m_SaveScriptButton, &QPushButton::clicked, this, &PyScriptEditor::SaveScript);
    connect(m_LoadScriptButton, &QPushButton::clicked, this, &PyScriptEditor::LoadScript);

    connect(m_ClearOutputButton, &QPushButton::clicked, this, [this]() { m_OutputConsole->clear(); });
}

void PyScriptEditor::Setup()
{
    QFont courier("Courier");
    m_InputConsole->setFont(courier);

    /* Outconsole formatting */
    m_OutputConsole->setFont(courier);
    m_OutputConsole->setReadOnly(true);

    QPalette p = m_OutputConsole->palette();
    p.setColor(QPalette::Text, palette().color(QPalette::Text).darker(150));
    m_OutputConsole->setPalette(p);

    /* Setup syntax highlighting */
    m_SyntaxHighlighter = new PySyntaxHighlighter(m_InputConsole->document());
}

void PyScriptEditor::ExecuteAll()
{
    m_OutputConsole->appendPlainText(m_InputConsole->toPlainText());
    m_Executor->Execute(m_InputConsole->toPlainText().toStdString()); 
}

void PyScriptEditor::ExecuteSelection()
{
    m_OutputConsole->appendPlainText(m_InputConsole->textCursor().selectedText());

    const std::string& selected = m_InputConsole->SelectedText();
    /**
     * If the code has whitespace or brackets/braces, it surely is a statement and has to be exec'd
     * on the contrary, if the code does not have it, it most likely is a case
     * where the user has selected a variable or member and just wants to see its value
     * and eval' could be the faster way
     */
    if (PotentialStatement(selected))
        m_Executor->Execute(selected);
    else
        m_Executor->Evaluate(selected);
}

void PyScriptEditor::SaveScript()
{
    QString filename = QFileDialog::getSaveFileName(this, "Save Python Script", "", "Python Files (*.py)");

    if (filename.isEmpty())
        return;

    QFile f(filename);
    if (f.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream text(&f);

        text << m_InputConsole->toPlainText();
        f.close();
    }
    else
    {
        VOID_LOG_ERROR("Could Not Save file");
    }
}

void PyScriptEditor::LoadScript()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open Python Script", "", "Python Files (*.py)");

    if (filename.isEmpty())
        return;
    
    QFile f(filename);
    if (f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream text(&f);

        m_InputConsole->clear();
        m_InputConsole->setPlainText(text.readAll());
    }
    else
    {
        VOID_LOG_ERROR("Could Not Open File");
    }
}

VOID_NAMESPACE_CLOSE
