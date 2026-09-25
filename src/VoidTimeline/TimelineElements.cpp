// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QAbstractItemView>
#include <QAction>
#include <QLineEdit>
#include <QMenu>
#include <QPalette>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QValidator>

/* Internal */
#include "TimelineElements.h"
#include "VoidCore/Logging.h"
#include "VoidCore/Timekeeper.h"
#include "VoidIconForge/IconForge.h"

VOID_NAMESPACE_OPEN

/* Loop Type Button {{{ */

LoopTypeButton::LoopTypeButton(QWidget* parent)
	: QPushButton(parent)
	, m_LoopType(LoopType::LoopInfinitely)
{
	/* Update Loop States to be used on the button */
	m_LoopState[LoopType::LoopInfinitely] = {"Loop Infinitely", IconType::icon_repeat};
	m_LoopState[LoopType::LoopOneInfinitely] = {"Loop One", IconType::icon_repeat_one};
	m_LoopState[LoopType::PingPong] = {"Bounce", IconType::icon_sync_alt};
	m_LoopState[LoopType::PlayOnce] = {"Play Once", IconType::icon_trending_flat};

	/* Update to set the state on the button */
	Build();

	/* No Visible borders */
	setFlat(true);
}

LoopTypeButton::~LoopTypeButton()
{
	m_Menu->deleteLater();
}

void LoopTypeButton::SetLoopType(const LoopType& looptype)
{
	m_LoopType = looptype;
	Update();

	emit loopTypeChanged(looptype);
}

void LoopTypeButton::Build()
{
	Update();
	m_Menu = new QMenu(this);

	for (const std::pair<LoopType, LoopState>& entry: m_LoopState)
	{
		QIcon icon;
		icon.addPixmap(IconForge::GetPixmap(entry.second.icon, _DARK_COLOR(QPalette::Text, 120), 18), QIcon::Normal);
		icon.addPixmap(IconForge::GetPixmap(entry.second.icon, _COLOR(QPalette::Dark), 18), QIcon::Active);

		QAction* action = new QAction(icon, entry.second.text.c_str(), m_Menu);
		connect(action, &QAction::triggered, this, [this, entry]() { SetLoopType(entry.first); });
		m_Menu->addAction(action);
	}

	setMenu(m_Menu);
}

void LoopTypeButton::Update()
{
	setIcon(IconForge::GetIcon(m_LoopState.at(m_LoopType).icon, _DARK_COLOR(QPalette::Text, 120)));
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

/* Framerate Box {{{ */

FramerateBox::FramerateBox(QWidget* parent)
	: QComboBox(parent)
{
	/* Validator */
	m_DoubleValidator = new QDoubleValidator(this);
	m_DoubleValidator->setBottom(0.0);
	m_DoubleValidator->setTop(2000.0);

	/* Setup the Box */
	Setup();

	connect(this, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [this](int) { RateChanged(currentText()); });
	connect(lineEdit(), &QLineEdit::returnPressed, this, [this]() { RateChanged(currentText()); });
	connect(lineEdit(), &QLineEdit::editingFinished, this, [this]() { RateChanged(currentText()); });

	lineEdit()->setAlignment(Qt::AlignCenter);
}

void FramerateBox::Setup()
{
	// Setup values and defaults
	const QStringList values = {
		"8",
		"10",
		"12",
		"12.50",
		"15",
		"23.98",
		"24",
		"25",
		"29.97",
		"30",
		"48",
		"50",
		"59.94",
		"60"
	};

	addItems(values);
	setEditable(true);

	setValidator(m_DoubleValidator);
	setFocusPolicy(Qt::ClickFocus);

	QPalette p = palette();
	p.setColor(QPalette::Base, p.color(QPalette::Dark));
    setPalette(p);
}

void FramerateBox::RateChanged(const QString& text)
{
	Timekeeper::Instance().SetFramerate(text.toDouble());
	emit framerateChanged(text.toDouble());

	/* Unset Focus once the text is updated */
	clearFocus();
}

/* }}} */

/// TimeEdit

TimeEdit::TimeEdit(QWidget* parent)
	: QLineEdit(parent)
{
	Setup();

	// Connections
	connect(this, &QLineEdit::returnPressed, this, [this]() -> void { emit frameEdited(Frame()); clearFocus(); });
	connect(this, &QLineEdit::editingFinished, this, [this]() -> void { emit frameEdited(Frame()); });
}

int TimeEdit::Frame() const
{
	QString t = text();
	return t.isEmpty() ? 0 : t.toInt();
}

void TimeEdit::Setup()
{
	setValidator(new QIntValidator(this));
	setAlignment(Qt::AlignCenter);

    QPalette p = palette();
    p.setColor(QPalette::Text, p.color(QPalette::Highlight));
	p.setColor(QPalette::Base, p.color(QPalette::Dark));
    setPalette(p);

    QFont f = font();
    f.setBold(true);
    setFont(f);

	setFocusPolicy(Qt::ClickFocus);
}

/// FrameDisplayModeBox

class DescriptiveItemDelegate : public QStyledItemDelegate
{
public:
	explicit DescriptiveItemDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}
	void initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const override
	{
		QStyledItemDelegate::initStyleOption(option, index);

		const QString description = index.data(Qt::UserRole + 1010).toString();
		if (description.isEmpty()) return;
		option->text = QString("%1 (%2)").arg(index.data(Qt::DisplayRole).toString(), description);
	}

	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override
	{
		QSize size = QStyledItemDelegate::sizeHint(option, index);
		return QSize(140, 24);
	}
};

FrameDisplayModeBox::FrameDisplayModeBox(QWidget* parent)
	: ControlCombo(parent)
{
	Setup();
}

void FrameDisplayModeBox::Setup()
{
	QStandardItemModel* model = new QStandardItemModel(this);
	QStandardItem* i1 = new QStandardItem("TF");
	i1->setData("Timeline Frames", Qt::UserRole + 1010);
	model->appendRow(i1);

	QStandardItem* i2 = new QStandardItem("TC");
	i2->setData("Timecode", Qt::UserRole + 1010);
	model->appendRow(i2);

	setModel(model);
	setItemDelegate(new DescriptiveItemDelegate(this));

	// Need the same width on the Popup for it to cover the contents
	view()->setMinimumWidth(140);

	connect(this, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [this](int index) -> void
	{
		emit frameDisplayChanged(static_cast<FrameDisplayMode>(index));
	});
}

VOID_NAMESPACE_CLOSE
