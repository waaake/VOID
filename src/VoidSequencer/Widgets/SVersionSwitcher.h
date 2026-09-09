// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_VERSION_SWITCHER_H
#define _SEQUENCER_VERSION_SWITCHER_H

/* Qt */
#include <QVBoxLayout>

/* Internal */
#include "Definition.h"
#include "VoidViews/VersionView.h"
#include "VoidQExtensions/Dialog.h"

VOID_NAMESPACE_OPEN

class SVersionSwitcher : public TranslucentDialog
{
public:
    SVersionSwitcher(QWidget* parent = nullptr);
    ~SVersionSwitcher();

    void ResetModel(EntityModel* model) { m_View->ResetModel(model); }
    void SetElementName(const QString& name);

private:
    QVBoxLayout* m_Layout;
    VersionView* m_View;

private:
    void Build();
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_VERSION_SWITCHER_H
