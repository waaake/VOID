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
#include "VoidObjects/Media/MediaClip.h"

VOID_NAMESPACE_OPEN

class MediaEditor : public BaseEditor
{
public:
    explicit MediaEditor(const SharedMediaClip& clip, QWidget* parent = nullptr);
    ~MediaEditor();

    bool HasSameMedia(const SharedMediaClip& media) const { return m_Clip.get() == media.get(); }

private:
    QLineEdit* m_PathEdit;
    QLineEdit* m_RangeStartEdit;
    QLineEdit* m_RangeEndEdit;
    QComboBox* m_ColorspaceCombo;
    QComboBox* m_LayerCombo;
    SharedMediaClip m_Clip;

private:
    void Build();
    void Setup();
};

VOID_NAMESPACE_CLOSE

#endif // _MEDIA_EDITOR_H
