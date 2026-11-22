// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _TOOLBOX_FRAMERATE_ELEMENT_H
#define _TOOLBOX_FRAMERATE_ELEMENT_H

/* STD */
#include <string>

/* Qt */
#include <QComboBox>
#include <QDoubleValidator>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class FramerateModel;

class FramerateBox : public QComboBox
{
	Q_OBJECT
public:
	explicit FramerateBox(QWidget* parent = nullptr);

	inline double Framerate() const { return currentText().toDouble(); }

	void SetFramerate(const double rate);

signals:
	void framerateChanged(double);

protected:
	void keyPressEvent(QKeyEvent* event) override;

private: /* Members */
	FramerateModel* m_Model;
	QDoubleValidator* m_DoubleValidator;

private: /* Methods */
	void Setup();
	void RateChanged(double d);
	void RateEdited();
};

VOID_NAMESPACE_CLOSE

#endif // _TOOLBOX_FRAMERATE_ELEMENT_H
