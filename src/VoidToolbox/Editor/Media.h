// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _MEDIA_EDITOR_H
#define _MEDIA_EDITOR_H

/* Qt */
#include <QComboBox>
#include <QLineEdit>

/* Internal */
#include "Definition.h"
#include "Editor.h"
#include "Controls/Field.h"
#include "VoidObjects/Media/MediaClip.h"

VOID_NAMESPACE_OPEN

class MediaEditor : public BaseEditor
{
    Q_OBJECT
public:
    explicit MediaEditor(const SharedMediaClip& clip, QWidget* parent = nullptr);
    ~MediaEditor();

    bool HasSameMedia(const SharedMediaClip& media) const { return m_Clip.get() == media.get(); }

signals:
    void mediaUpdated(const SharedMediaClip& media, bool needsRecache = false);

private:
    QLineEdit* m_PathEdit;
    IntField* m_RangeStartEdit;
    IntField* m_RangeEndEdit;
    QComboBox* m_ColorspaceCombo;
    QComboBox* m_LayerCombo;
    DoubleField* m_PixelAspectEdit;
    SharedMediaClip m_Clip;

private:
    void Build();
    void Setup();
};

VOID_NAMESPACE_CLOSE

#endif // _MEDIA_EDITOR_H
