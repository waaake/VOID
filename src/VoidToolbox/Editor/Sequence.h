// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCE_EDITOR_H
#define _SEQUENCE_EDITOR_H

/* Qt */
#include <QComboBox>
#include <QLineEdit>

/* Internal */
#include "Definition.h"
#include "Editor.h"
#include "Controls/Field.h"
#include "VoidToolbox/Elements/Framerate.h"
#include "VoidObjects/Sequence/Sequence.h"

VOID_NAMESPACE_OPEN

class SequenceEditor : public BaseEditor
{
    Q_OBJECT
public:
    explicit SequenceEditor(const SharedPlaybackSequence& sequence, QWidget* parent = nullptr);

    bool HasSameSequence(const SharedPlaybackSequence& sequence) const { return m_Sequence.get() == sequence.get(); }

protected:
    void Refresh() override;

private:
    FramerateBox* m_FramerateCtrl;
    IntField* m_FrameStartEdit;

    SharedPlaybackSequence m_Sequence;

private:
    void Build();
    void Setup();
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCE_EDITOR_H
