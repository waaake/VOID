// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QListView>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Playlist/Playlist.h"

VOID_NAMESPACE_OPEN

class QueueView : public QListView
{
public:
    explicit QueueView(QWidget* parent = nullptr);
    ~QueueView();

    void Set(Playlist* playlist);
    void Clear();

protected:
    void dropEvent(QDropEvent* event) override;
};

VOID_NAMESPACE_CLOSE
