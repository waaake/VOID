// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _MEDIA_VERSION_VIEW
#define _MEDIA_VERSION_VIEW

/* Qt */
#include <QListView>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Models/EntityModel.h"

VOID_NAMESPACE_OPEN

class VOID_API VersionView : public QListView
{
    Q_OBJECT
public:
    explicit VersionView(QWidget* parent = nullptr);
    ~VersionView();

    int RowCount() const { return m_Proxy->rowCount(); }
    void ResetModel(EntityModel* model);
    void SetElementName(const QString& name);

signals:
    void versionChanged(const SharedMediaClip&);

private:
    MediaVersionProxyModel* m_Proxy;
};

VOID_NAMESPACE_CLOSE

#endif // _MEDIA_VERSION_VIEW
