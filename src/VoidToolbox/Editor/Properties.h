// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _PROPERTIES_EDITOR_H
#define _PROPERTIES_EDITOR_H

/* Qt */
#include <QWidget>
#include <QLayout>
#include <QPushButton>

/* Internal */
#include "Definition.h"
#include "Effects.h"
#include "Media.h"
#include "VoidObjects/Sequence/Sequence.h"
#include "VoidQExtensions/LineEdit.h"

VOID_NAMESPACE_OPEN

class ColorEngine;

class VOID_API PropertiesPanel : public QWidget
{
    Q_OBJECT
public:
    explicit PropertiesPanel(QWidget* parent = nullptr);
    ~PropertiesPanel();

    QSize sizeHint() const override { return QSize(500, 720); }

    void Show(QWidget* panel);
    void EditEffect(Effect* effect);
    void EditMedia(const SharedMediaClip& media);
    void EditSequence(const SharedPlaybackSequence& sequence);
    void Inspect(ColorEngine* engine);

    void Clear();
    void Remove(const Effect* effect);
    void ClearAdditionalPanels(int limit);

signals:
    void mediaReceivedUpdate(const SharedMediaClip&, bool needsRecache = false);

private: /* Members */
    QVBoxLayout* m_Layout;
    QVBoxLayout* m_ScrollLayout;
    QPushButton* m_ClearButton;
    IntBoundLineEdit* m_PanelCounter;

private: /* Methods */
    void Build();
    void Setup();
    void Connect();
};

VOID_NAMESPACE_CLOSE

#endif // _PROPERTIES_EDITOR_H
