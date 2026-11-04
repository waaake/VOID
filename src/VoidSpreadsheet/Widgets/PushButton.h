// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SPREADSHEET_PUSH_BUTTON_H
#define _SPREADSHEET_PUSH_BUTTON_H

/* Qt */
#include <QPushButton>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Media/MediaClip.h"

VOID_NAMESPACE_OPEN

/**
 * @brief Provides a QPushbutton that allows media to be dragged on top of it to emit
 * a signal with the dropped MediaClips
 * 
 */
class MediaDropButton : public QPushButton
{
    Q_OBJECT
public:
    explicit MediaDropButton(const QString& text, QWidget* parent = nullptr);

signals:
    void mediaDropped(const std::vector<SharedMediaClip>&);

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
};

VOID_NAMESPACE_CLOSE

#endif // _SPREADSHEET_PUSH_BUTTON_H
