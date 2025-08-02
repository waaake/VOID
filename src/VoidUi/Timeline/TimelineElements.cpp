/* Qt */
#include <QAction>
#include <QMenu>
#include <QPalette>
#include <QValidator>

/* Internal */
#include "TimelineElements.h"

VOID_NAMESPACE_OPEN

/* Loop Type Button {{{ */

LoopTypeButton::LoopTypeButton(QWidget* parent)
	: QPushButton(parent)
	, m_LoopType(LoopType::LoopInfinitely)
{
	/* Update Loop States to be used on the button */
	m_LoopState[LoopType::LoopInfinitely] = {"Loop Infinitely", ":resources/icons/icon_repeat.svg"};
	m_LoopState[LoopType::PingPong] = {"Bounce", ":resources/icons/icon_bounce.svg"};
	m_LoopState[LoopType::PlayOnce] = {"Play Once", ":resources/icons/icon_arrow_right.svg"};

	/* Update to set the state on the button */
	Build();

	/* No Visible borders */
	setFlat(true);
}

LoopTypeButton::~LoopTypeButton()
{
	/* Delete the Menu and all it's children */
	m_Menu->deleteLater();
}

void LoopTypeButton::SetLoopType(const LoopType& looptype)
{
	/* Update Loop state */
	m_LoopType = looptype;

	/* Reset Icon */
	Update();

	/* Emit that the loop type has been changed */
	emit loopTypeChanged(looptype);
}

void LoopTypeButton::Build()
{
	/* Update the state on the button */
	Update();

	/* Add menu for Loop Type Selection */
	m_Menu = new QMenu(this);

	for (std::pair<LoopType, LoopState> entry: m_LoopState)
	{
		/* Action for the menu */
		QAction* action = new QAction(entry.second.text.c_str(), m_Menu);

		/* Connect the action to set the Loop State */
		connect(action, &QAction::triggered, this, [this, entry]() { SetLoopType(entry.first); });

		/* Add to the Menu */
		m_Menu->addAction(action);
	}

	/* Set the Menu on the button */
	setMenu(m_Menu);
}

/* }}} */

/* TimeDisplay {{{ */

TimeDisplay::TimeDisplay(QWidget* parent)
    : QLabel(parent)
{
    Setup();
}

void TimeDisplay::Setup()
{
    /* How the UI Apears */
	setEnabled(false);
	setAlignment(Qt::AlignCenter);

    /* Fixed Size? */
	setFixedWidth(80);

    /* Setup the Colors */
    QPalette p = palette();
    p.setColor(QPalette::WindowText, p.color(QPalette::Highlight));
    setPalette(p);
    
    /* Setup Font */
    QFont f = font();
    f.setBold(true);
    setFont(f);
}

/* }}} */

VOID_NAMESPACE_CLOSE
