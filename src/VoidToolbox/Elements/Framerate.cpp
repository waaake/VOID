// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QKeyEvent>
#include <QLineEdit>
#include <QPalette>

/* Internal */
#include "Framerate.h"
#include "VoidCore/Timekeeper.h"
#include "VoidObjects/Models/Framerate.h"

#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

FramerateBox::FramerateBox(QWidget* parent)
	: QComboBox(parent)
{
	m_Model = new FramerateModel(this);

	m_DoubleValidator = new QDoubleValidator(this);
	m_DoubleValidator->setRange(0.1, 1000.0, 6);

	Setup();

	connect(this, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [this](int index)
	{
		RateChanged(m_Model->Framerate(index));
	});
	connect(lineEdit(), &QLineEdit::returnPressed, this, &FramerateBox::RateEdited);
	connect(lineEdit(), &QLineEdit::editingFinished, this, &FramerateBox::RateEdited);

	lineEdit()->setAlignment(Qt::AlignCenter);
}

void FramerateBox::SetFramerate(double framerate)
{
    /// Might be a bit expensive in case the framerate doesn't already exist
    /// Need to see if we can do this in a better way
    QModelIndex index = m_Model->Index(framerate);
    if (!index.isValid())
    {
        m_Model->Add(framerate);
        index = m_Model->Index(framerate);
    }

    setCurrentIndex(index.row());
}

void FramerateBox::keyPressEvent(QKeyEvent* event)
{
	/// This happens before the lineEdit::returnPressed is invoked
	/// if the value is not added to the model by then, it's set as empty
	/// we have one way to override and accept the event, let it not propagate further down
	/// or we could do like this, add the value to the model, so by the time returnPressed is invoked,
	/// we already have this value persist and that will invoke the required signals...
	if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
		m_Model->Add(currentText());

	QComboBox::keyPressEvent(event);
}

void FramerateBox::Setup()
{
	setModel(m_Model);
	setEditable(true);

	setValidator(m_DoubleValidator);
	setFocusPolicy(Qt::ClickFocus);

	QPalette p = palette();
	p.setColor(QPalette::Base, p.color(QPalette::Dark));
    setPalette(p);

}

void FramerateBox::RateChanged(double d)
{
	Timekeeper::Instance().SetFramerate(d);
	emit framerateChanged(d);

	clearFocus();
}

void FramerateBox::RateEdited()
{
	const double t = currentText().toDouble();
	// Ensure that we have it added to the model as well
	m_Model->Add(t);
	RateChanged(t);
}

VOID_NAMESPACE_CLOSE
