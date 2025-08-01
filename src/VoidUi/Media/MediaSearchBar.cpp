/* Internal */
#include "MediaSearchBar.h"

VOID_NAMESPACE_OPEN

MediaSearchBar::MediaSearchBar(QWidget* parent)
    : QLineEdit(parent)
{
    /* Setup UI */
    Setup();

    /* Connect Signals */
    Connect();
}

MediaSearchBar::~MediaSearchBar()
{
    m_ClearAction->deleteLater();
    delete m_ClearAction;
    m_ClearAction = nullptr;
}

void MediaSearchBar::Setup()
{
    setPlaceholderText("Search");

    /* Add the Clear Action */
    m_ClearAction = new QAction(QIcon(":resources/icons/icon_close.svg"), "", this);
    m_ClearAction->setToolTip("Clears Search field");

    /* Add at the end */
    addAction(m_ClearAction, QLineEdit::TrailingPosition);
    /* Set Hidden by default */
    m_ClearAction->setVisible(false);

    /* Focus Policy */
    setFocusPolicy(Qt::ClickFocus);
}

void MediaSearchBar::Connect()
{
    connect(this, &QLineEdit::textChanged, this, &MediaSearchBar::TextChanged);
    connect(m_ClearAction, &QAction::triggered, this, &QLineEdit::clear);
}

void MediaSearchBar::TextChanged(const QString& text)
{
    /* The clear action is visible only if the */
    m_ClearAction->setVisible(!text.isEmpty());

    /* Emit that something was typed */
    emit typed(text.toStdString());
}

VOID_NAMESPACE_CLOSE
