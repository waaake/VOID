// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QWidget>
#include <QLayout>
#include <QPushButton>

/* Internal */
#include "Definition.h"
#include "VoidUi/Media/Views/QueueView.h"

VOID_NAMESPACE_OPEN

class MediaQueue : public QWidget
{
public:
    explicit MediaQueue(QWidget* parent = nullptr);
    ~MediaQueue();

    void Set(Playlist* playlist);
    void Clear() { m_View->Clear(); }

private: /* Members */
    QVBoxLayout* m_Layout;
    QPushButton* m_MoveUpArrow;
    QPushButton* m_MoveDownArrow;
    QueueView* m_View;

    Playlist* m_Playlist;

private: /* Methods */
    void Build();
    void Setup();
    void Connect();
};

VOID_NAMESPACE_CLOSE
