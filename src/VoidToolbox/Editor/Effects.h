// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _EFFECTS_EDITOR_H
#define _EFFECTS_EDITOR_H

/* Internal */
#include "Definition.h"
#include "Editor.h"
#include "VoidObjects/Effects/Effects.h"
#include "VoidQExtensions/PushButton.h"

VOID_NAMESPACE_OPEN

class EffectEditor : public BaseEditor
{
    Q_OBJECT
public:
    EffectEditor(Effect* effect, QWidget* parent = nullptr);

    bool HasSameEffect(const Effect* effect) const { return effect == m_Effect; }

private:
    Effect* m_Effect;

private: /* Methods */
    void Build();
    void Setup();

    // Param Setup
    void SetupIntParam(const Param* param);
    void SetupFloatRangedParam(const Param* param);
    void SetupFloatParam(const Param* param);
    void SetupBoolParam(const Param* param);
    void SetupStringParam(const Param* param);
};

VOID_NAMESPACE_CLOSE

#endif // _EFFECTS_EDITOR_H
