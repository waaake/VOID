#ifndef _VOID_Q_EXT_LABEL_H
#define _VOID_Q_EXT_LABEL_H

/* Qt */
#include <QLabel>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

/**
 * A Label that emits 'clicked' signal when clicked on.
 */
class ClickableLabel : public QLabel
{
    Q_OBJECT

public:
    ClickableLabel(QWidget* parent = nullptr);
    ClickableLabel(const QString& text, QWidget* parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent* event) override;

signals:
    void clicked();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_Q_EXT_LABEL_H
