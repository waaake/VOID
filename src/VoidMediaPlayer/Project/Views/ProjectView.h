// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_PROJECT_VIEW_H
#define _VOID_PROJECT_VIEW_H

/* Qt */
#include <QAction>
#include <QListView>
#include <QMenu>

/* Internal */
#include "QDefinition.h"
#include "VoidMediaPlayer/Media/MediaBridge.h"

VOID_NAMESPACE_OPEN

class ProjectView : public QListView
{
    Q_OBJECT

public:
    explicit ProjectView(QWidget* parent = nullptr);
    ~ProjectView();

    // Search and filter items from the Model
    inline void Search(const std::string& text) { proxy->SetSearchText(text); }

    // Returns the currently selected Project row Model Indices
    const std::vector<QModelIndex> SelectedIndexes() const;

    // Returns if the widget has selection
    bool HasSelection();

    // Toggle sorting on the Model
    void EnableSorting(bool state, const Qt::SortOrder& order = Qt::AscendingOrder);

signals:
    void itemClicked(const QModelIndex&);

private:
    ProjectProxyModel* proxy;

    QMenu* m_NewMenu;
    QMenu* m_ImportMenu;
    QAction* m_ImportMediaAction;
    QAction* m_ImportDirectoryAction;
    QAction* m_CreateSequenceAction;
    QAction* m_SaveProjectAction;
    QAction* m_SaveAsProjectAction;
    QAction* m_CloseProjectAction;

private: /* Methods */
    void Setup();
    void Connect();
    void ResetModel(ProjectModel* model);
    void ItemClicked(const QModelIndex& index);
    void ShowContextMenu(const _QPoint& position);
    Project* HighlightedProject();
    void ImportMedia();
    void ImportDirectory();
    void AddSequence();
    void SaveProject(bool saveas = false);
    void CloseProject();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PROJECT_VIEW_H
