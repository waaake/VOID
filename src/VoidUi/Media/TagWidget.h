// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QLayout>
#include <QLineEdit>
#include <QListView>
#include <QPushButton>
#include <QTreeView>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Media/MediaClip.h"
#include "VoidUi/QExtensions/Dialog.h"

VOID_NAMESPACE_OPEN

class TagWidget;
class TagEditor;

class TagBase : public QWidget
{
public:
    TagBase(QWidget* parent = nullptr);
    ~TagBase();

    void Reset();

private: /* Members */
    QGridLayout* m_Layout;
    QLineEdit* m_NameEdit;
    QTreeView* m_DataTree;

private: /* Methods */
    void Build();
    void Setup();

    friend class TagWidget;
    friend class TagEditor;
};

class TagWidget : public TranslucentDialog
{
public:
    TagWidget(const SharedMediaClip& clip, QWidget* parent = nullptr);
    ~TagWidget();

    void MoveTo(const QPoint& position);

protected:
    void showEvent(QShowEvent* event) override;

private: /* Members */
    QVBoxLayout* m_Layout;

    TagBase* m_TagBase;
    QPushButton* m_AcceptButton;

    std::weak_ptr<MediaClip> m_Media;
    TagMetadataModel* m_Metadata;

private: /* Methods */
    void Build();
    void Setup();
    void Connect();
};

class TagEditor : public TranslucentDialog
{
public:
    TagEditor(const SharedMediaClip& clip, QWidget* parent = nullptr);
    ~TagEditor();

    void MoveTo(const QPoint& position);

private: /* Members */
    QVBoxLayout* m_Layout;

    QListView* m_TagList;
    TagBase* m_TagBase;
    QPushButton* m_RemoveButton;

    std::weak_ptr<MediaClip> m_Media;

private: /* Methods */
    void Build();
    void Setup();
    void Connect();

    void RemoveSelected();
    void TagSelected(const QModelIndex& index);
    void SetCurrentTag(const Tag* tag);
};

VOID_NAMESPACE_CLOSE
