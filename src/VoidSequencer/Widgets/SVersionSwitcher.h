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
#include "VoidObjects/Sequence/TrackItem.h"
#include "VoidSequencer/SContext.h"

VOID_NAMESPACE_OPEN

class SVersionSwitcher : public TranslucentDialog
{
public:
    SVersionSwitcher(SequencerContext* context, QWidget* parent = nullptr);
    ~SVersionSwitcher();

    void ResetModel(EntityModel* model) { m_View->ResetModel(model); }
    void Exec(const SharedTrackItem& item, const QPoint& position);

private:
    QVBoxLayout* m_Layout;
    VersionView* m_View;

    SequencerContext* m_Context;
    SharedTrackItem m_Item;

private:
    void Build();
    void SetElementName(const QString& name);
    void ResetMedia(const QModelIndex& index);
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_VERSION_SWITCHER_H
