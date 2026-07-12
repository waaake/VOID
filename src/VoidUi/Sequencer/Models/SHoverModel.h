// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_HOVER_MODEL_H
#define _SEQUENCER_HOVER_MODEL_H

/* Qt */
#include <QObject>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Sequence/TrackItem.h"

VOID_NAMESPACE_OPEN

class SHoverModel : public QObject
{
    Q_OBJECT
public:
    explicit SHoverModel(QObject* parent = nullptr);

    void Set(const SharedTrackItem& item);
    void Reset();
    bool IsHovered(const SharedTrackItem& item) const { return item.get() == m_Hovered.get(); }
    SharedTrackItem Current() const { return m_Hovered; }
    
signals:
    void hoverChanged(const SharedTrackItem& current, const SharedTrackItem& previous);

private:
    SharedTrackItem m_Hovered;
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_HOVER_MODEL_H
